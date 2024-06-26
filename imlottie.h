/*
 * Base code written by
 * dalerank (dalerankn8@gmail.com) - code adaption, main logic
 * anatolii (anatoliigr@gmail.com) - render library fixes, pipeline redesign
 * cybercuzma2077 (cybercuzma2077@gmail.com) - porting to unreal engine
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include <inttypes.h>

#include <mutex>
#include <queue>
#include <string>
#include <unordered_map>
#include <cassert>

#include "imgui.h"
#include "imgui_internal.h"

#ifdef IMLOTTIE_DX11_IMPLEMENTATION
#include <d3d11.h>
#endif // IMLOTTIE_DX11_IMPLEMENTATION

#ifdef IMLOTTIE_OPENGL_IMPLEMENTATION
#ifdef HELLOIMGUI_HAS_OPENGL
#include "hello_imgui/hello_imgui_include_opengl.h"
#else
#include <glad/glad.h>
#endif
#endif

namespace imlottie { 
    class Animation;
    std::shared_ptr<imlottie::Animation> animationLoad(const char *path);
    uint16_t animationTotalFrame(const std::shared_ptr<imlottie::Animation> &anim);
    double animationDuration(const std::shared_ptr<imlottie::Animation> &anim);
    void animationRenderSync(const std::shared_ptr<imlottie::Animation> &anim, int nextFrameIndex, uint32_t *data, int width, int height, int row_pitch);
}

namespace ImLottie {

constexpr ImGuiID BAD_PICTUREID = ImGuiID(-1);

// Data in system memory where saved frame
struct NextFrame {
    std::vector<uint8_t> data;
    ImVec2 size;
};

// Data in system memory, this frame ready for move to tmp atlas
struct ReadyFrame {
    ImGuiID pid = BAD_PICTUREID;
    std::vector<uint8_t> data;
    ImVec2 size;
#if DEBUG_LOTTIE_UPDATE
    const char *lottie = nullptr;
    int frame = 0;
    int duration_ms = 0;
#endif
};

class LottieAnimationRenderer;
namespace detail {
    LottieAnimationRenderer *g_lottieRenderer = nullptr;
}

// This code defines a struct called LottieAnim, which represents a Lottie animation. It contains various static constants,
// such as the default size and pre-rendered frames, as well as variables that store information about the animation
// such as the timeline and frames.
struct LottieAnim {
    // default size for lottie renderer
    static constexpr int DEFAULT_SIZE = 32;
    // how many prerendered frames saved in array
    static constexpr int DEFAULT_PRERENDERED_FRAMES = 2;
    static constexpr int LOTTIE_SURFACE_FMT = sizeof(uint32_t); // TEXFMT_A8R8G8B8;
    static constexpr int LOTTIE_SURFACE_FMT_BPP = sizeof(uint32_t);

    // A unique identifier for the picture
    ImGuiID pid = BAD_PICTUREID;

#ifdef IMLOTTIE_DX11_IMPLEMENTATION
    // DirectX 11 implementation
    ID3D11Texture2D* texture = nullptr;
    ID3D11ShaderResourceView *srv = nullptr;
#endif // IMLOTTIE_DX11_IMPLEMENTATION
#ifdef IMLOTTIE_OPENGL_IMPLEMENTATION
    bool texture = false;
    GLuint srv = 0;
#endif
    struct {
        int width = DEFAULT_SIZE;
        int height = DEFAULT_SIZE;
    } canvas;

    struct {
        uint32_t duration_ms = 0;
        uint32_t last_ms = 0;
    } timeline;

    struct {
        uint16_t current = 0;
        uint16_t total = 0;
    } frame;

    // Flags for the animation
    bool loop = false;
    bool play = false;
    bool renderonce = false;

    int maxPrerenderedFrames = DEFAULT_PRERENDERED_FRAMES;
    std::string lottiePath;

    std::shared_ptr<imlottie::Animation> anim;
    // we need save future frames, because are can have
    // different time for render, thread render it on loop
    std::queue<NextFrame> prerenderedFrames;

    // here saved frame, which need for display, on every render()
    // call prerendered frame will moved here when time for next frame gone
    ReadyFrame currentFrame;
#ifdef IMLOTTIE_SIMPLE_IMPLEMENTATION
    bool currentFrameRendering = true;
#endif

    // Grabs the current frame and stores it in the "f" parameter
    bool grabCurrentFrame(ReadyFrame &f) {
        if (currentFrame.pid == BAD_PICTUREID) {
            return false;
        }

        std::swap(f, currentFrame);
        return true;
    }

    // Returns a hash code based on the properties of the Lottie animation
    static ImGuiID getPropsHash(const char *lottie, const int canvasWidth, const int canvasHeight, bool loop, int rate) {
        char hash[512];
        snprintf(hash, 511, "lottie:%s|canvasHeight:%d|canvasWidth:%d|loop:%d|rate:%d", lottie, canvasWidth, canvasHeight, loop, rate);
        return ImHashStr(hash, 0, 0xc001f00d);
        ;
    }

    // Loads the Lottie animation from the specified file path
    bool load(const char *path, int w, int h, bool _loop, bool _play, int _prerenderedFrames, int rate, ImGuiID _pid) {
        if (!path || 0 == *path) {
            return false;
        }

        canvas.width = std::max<int>(w, DEFAULT_SIZE);
        canvas.height = std::max<int>(h, DEFAULT_SIZE);

        loop = _loop;
        play = _play;
        pid = _pid;
        maxPrerenderedFrames = std::max<int>(_prerenderedFrames, DEFAULT_PRERENDERED_FRAMES);

        lottiePath = path;
        anim = imlottie::animationLoad(path);

        if (anim) {
            int customRate = rate;
            frame.total = (uint16_t)imlottie::animationTotalFrame(anim);
            float oneFrameMs = (float)imlottie::animationDuration(anim) / frame.total;
            timeline.duration_ms = int(customRate > 0 ? 1000 / customRate : oneFrameMs * 1000);
        } else {
            printf("Lottie::animation load failed from <%s>", path);
            return false;
        }

        return true;
    }
#ifdef IMLOTTIE_SIMPLE_IMPLEMENTATION
    bool updateCurtimeFrame(uint32_t curTime) {
        if (pid == BAD_PICTUREID || !(play || renderonce))
            return false;

        renderonce = false;
        if (!loop && frame.current > frame.total)
            return false;

        if ((curTime - timeline.last_ms) >= timeline.duration_ms) {
            currentFrameRendering = true;
            uint32_t frameDiff = (curTime - timeline.last_ms) / timeline.duration_ms;
            frame.current += frameDiff;
            timeline.last_ms += frameDiff * timeline.duration_ms;
            if (loop) {
                frame.current %= frame.total;
            }
            size_t bufferSize = canvas.width * canvas.height * LOTTIE_SURFACE_FMT_BPP;
            currentFrame.data.resize(bufferSize);
            imlottie::animationRenderSync(anim, frame.current, (uint32_t*)currentFrame.data.data(), canvas.width, canvas.height, canvas.width * LOTTIE_SURFACE_FMT_BPP);
            currentFrameRendering = false;
            return true;
        }
        return false;
    }
#endif

    bool render(uint32_t curTime) {
        if (pid == BAD_PICTUREID || !(play || renderonce))
            return false;

        renderonce = false;
        if (!loop && frame.current > frame.total)
            return false;

        uint32_t frameDiff = (curTime - timeline.last_ms) / timeline.duration_ms;
        if (frameDiff != 0) {
            // move first of prerendered frames to readyFrame, main thread
            // after render it will be move to readFrames array
            if (prerenderedFrames.size() > 0) {
                // move the first pre-rendered frame to the current frame
                NextFrame nextFrame;
                std::swap(nextFrame, prerenderedFrames.front());
                prerenderedFrames.pop();
                std::swap(currentFrame.data, nextFrame.data);
                currentFrame.size = nextFrame.size;
                currentFrame.pid = pid;
#if DEBUG_LOTTIE_UPDATE
                // for debugging purposes, set the lottie path, current frame and duration
                currentFrame.lottie = lottiePath.c_str();
                currentFrame.frame = frame.current;
                currentFrame.duration_ms = timeline.duration_ms;
#endif // DEBUG_LOTTIE_UPDATE
            }

            // switch to next frame index
            frame.current++;
            if (loop) {
                frame.current %= frame.total;
            }
            timeline.last_ms += frameDiff * timeline.duration_ms;
        }

        if (prerenderedFrames.size() <= std::max<int>(maxPrerenderedFrames, DEFAULT_PRERENDERED_FRAMES)) {
            // calc next prerendered frame index
            uint16_t nextFrameIndex = frame.current + (uint16_t)prerenderedFrames.size();

            // if loop we need back to 0 and render again
            if (loop) {
                nextFrameIndex = (nextFrameIndex % frame.total);
            }

            // not need prerender frames when all finished
            if (nextFrameIndex < frame.total) {
                // create new frame
                prerenderedFrames.push({});
                NextFrame &nextFrame = prerenderedFrames.back();

                // size for next frame memory
                size_t bufferSize = canvas.width * canvas.height * LOTTIE_SURFACE_FMT_BPP;

                // create memory block where will be placed frame
                nextFrame.data.resize(bufferSize);

                // save frame size for next actions
                nextFrame.size = ImVec2((float)canvas.width, (float)canvas.height);

                imlottie::animationRenderSync(anim, nextFrameIndex, (uint32_t *)nextFrame.data.data(), canvas.width, canvas.height, canvas.width *LOTTIE_SURFACE_FMT_BPP);
                return true;
            }
        }

        return false;
    }

    // Simple helper function to load an image into a DX11 texture with common settings
#ifdef IMLOTTIE_DX11_IMPLEMENTATION
    bool createTextureFromData(uint8_t *image_data, ::ID3D11Device* pd3dDevice) {
        if (image_data == NULL) {
            return false;
        }

        // Create texture
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Width = canvas.width;
        desc.Height = canvas.height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.Usage = D3D11_USAGE_DYNAMIC;

        D3D11_SUBRESOURCE_DATA subResource;
        subResource.pSysMem = image_data;
        subResource.SysMemPitch = desc.Width * 4;
        subResource.SysMemSlicePitch = 0;
        pd3dDevice->CreateTexture2D(&desc, &subResource, &texture);

        // Create texture view
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;
        pd3dDevice->CreateShaderResourceView(texture, &srvDesc, &srv);
        //pTexture->Release();
        return true;
    }

    bool updateTextureFromData(unsigned char* image_data, ID3D11DeviceContext* ctx) {
        if (!image_data) {
            return false;
        }

        D3D11_MAPPED_SUBRESOURCE ms;
        HRESULT hr = ctx->Map(texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
        if (FAILED(hr)) {
            return false;
        }

        const uint8_t* src = image_data;
        uint8_t* dst = (uint8_t*)ms.pData;

        uint32_t bytes_per_row = canvas.width * 4;
        for (int y = 0; y < canvas.height; ++y) {
            memcpy(dst, src, bytes_per_row);
            src += bytes_per_row;
            dst += ms.RowPitch;
        }

        ctx->Unmap(texture, 0);
        return true;
    }
#endif // IMLOTTIE_DX11_IMPLEMENTATION

#ifdef IMLOTTIE_OPENGL_IMPLEMENTATION
    bool createTextureFromData(uint8_t *image_data) {
        if (image_data == NULL) {
            return false;
        }
        auto& self = *this;
        glGenTextures(1, &srv);
        glBindTexture(GL_TEXTURE_2D, srv);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#if defined(HELLOIMGUI_USE_GLES2) || defined(HELLOIMGUI_USE_GLES3)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#endif
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     canvas.width,
                     canvas.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        glBindTexture(GL_TEXTURE_2D, 0);
        texture = true;
        return glGetError() == GL_NO_ERROR;
    }

    bool updateTextureFromData(unsigned char* image_data)
    {
        if (image_data == NULL) {
            return false;
        }

        // glDeleteTextures(1, &srv);
        // return createTextureFromData(image_data);
        glBindTexture(GL_TEXTURE_2D, srv);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                     canvas.width,
                     canvas.height, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        glBindTexture(GL_TEXTURE_2D, 0);
        return glGetError() == GL_NO_ERROR;
    }
#endif
};

struct LottieRenderCommand {
    enum Type { UNKNOWN = 0, ADD_CONFIG, DISCARD_PID, SETUP_PID, SETUP_PLAY, SETUP_RENDER };
    Type type;
    std::string path;
    int w, h;
    int loop;
    int rate;
    ImGuiID pid;
    bool play;
    bool render;
};

// this thread resolve command to load lotti animations, and their render frames
struct LottieRenderThread {
    std::atomic_int terminating = false;
    bool popCommand(LottieRenderCommand &command) {
        std::lock_guard<std::mutex> lock(commandsMutex);
        if (commands.empty())
            return false;

        std::swap(command, commands.front());
        commands.pop();
        return true;
    }

    void addCommand(const LottieRenderCommand &command) {
        if (commands.size() > 100) {
            return;
        }
        std::lock_guard<std::mutex> lock(commandsMutex);
        commands.push(command);
    }

    size_t getCommandNum()
    {
        return commands.size();
    }

    std::thread independentThread;
    std::unordered_map<uint32_t, LottieAnim> animations;

    // this queue contain commands for animations
    // load - load animation may take much time
    // discard - after reset\remove image in PM we need remove it from quese
    //           because PM cant contain real image for it
    // setup pid - animation need assign PictureID in PM, this pid
    //             received later than load, when we have area in atlas for image
    // setup play flag - for future, when we need change play status
    std::mutex commandsMutex;
    std::queue<LottieRenderCommand> commands;

    // this queue contain ready frames from animations, it placed in system
    // memory that another thread can copy their to PM texture later
    std::mutex readyFramesMutex;
    std::queue<ReadyFrame> readyFrames;
    float curtime = 0;

    void pushReadyFrame(ReadyFrame &frame, size_t maxAnimSize) {
        std::lock_guard<std::mutex> lock(readyFramesMutex);
        // remove extra frames, that avoid creating infinite queue
        if (readyFrames.size() > maxAnimSize)
            readyFrames.pop();

        readyFrames.push({});
        std::swap(readyFrames.back(), frame);
    }

    bool popReadyFrame(ReadyFrame &frame) {
        std::lock_guard<std::mutex> lock(readyFramesMutex);
        if (readyFrames.empty())
            return false;

        std::swap(frame, readyFrames.front());
        readyFrames.pop();
        return true;
    }

    // resolve command in thread, because it can be added async from another thread
    void resolveCommand(const LottieRenderCommand &cmd) {
        switch (cmd.type) {
        case LottieRenderCommand::ADD_CONFIG:
        {
            LottieAnim anim;
            bool loadOk = anim.load(cmd.path.c_str(), cmd.w, cmd.h, cmd.loop, true, 2, cmd.rate, cmd.pid);
            if (loadOk) {
                animations.insert({cmd.pid, std::move(anim)});
            }
        } break;

        case LottieRenderCommand::DISCARD_PID:
        {
            auto it = animations.find(cmd.pid);
            if (animations.find(cmd.pid) != animations.end()) {
                animations.erase(it);
            }
        } break;

        case LottieRenderCommand::SETUP_PID:
        {
            const uint32_t propsHash = LottieAnim::getPropsHash(cmd.path.c_str(), cmd.w, cmd.h, cmd.loop, cmd.rate);
            auto it = animations.find(propsHash);
            if (it != animations.end()) {
                it->second.pid = cmd.pid;
            }
        } break;

        case LottieRenderCommand::SETUP_PLAY:
        {
            auto it = std::find_if( animations.begin(), animations.end(), [pid = cmd.pid](auto &a) { return a.second.pid == pid; });
            if (it != animations.end()) {
                it->second.play = cmd.play;
            }
        } break;

        case LottieRenderCommand::SETUP_RENDER:
        {
            auto it = std::find_if( animations.begin(), animations.end(), [pid = cmd.pid](auto &a) { return a.second.pid == pid; });
            if (it != animations.end()) {
                it->second.renderonce = cmd.render;
            }
        } break;


        default:
        break;
        }
    }

    void execute() {
        while (!terminating.load()) {
            LottieRenderCommand cmd;
            if (popCommand(cmd)) {
                resolveCommand(cmd);
            }

            if (animations.empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000/30));
            }

            // render animations and extract current animation frame to ready frames array
            const size_t maxAnimSize = animations.size() * 2;
            for (auto &anim : animations) {
                // it's loop here for all animations and frame render make a time, break
                // it when thread want stop
                if (terminating.load())
                    return;

                // prerender next frames and prepare copy data to current frame if need
                anim.second.render((uint32_t)curtime);

                // if current frame ready, we need copy it to ready frames array
                // ready frames array will be copied to dynatlas on frame update from
                // main thread so we need use mutex for guard access when array changes
                ReadyFrame currentFrame;
                if (anim.second.grabCurrentFrame(currentFrame)) {
                    pushReadyFrame(currentFrame, maxAnimSize);
                }
            }
        }
    }

#ifdef IMLOTTIE_SIMPLE_IMPLEMENTATION
    void simpleExecute() {
        uint32_t lasttime = 0;
        size_t commandNum = 0;
        LottieRenderCommand cmd;
        while (!terminating.load()) {
            if (commandNum != getCommandNum()) {
                if (popCommand(cmd)) {
                    resolveCommand(cmd);
                }
            }

            if (animations.empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            if (lasttime != curtime) {
                for (auto& [key, value] : animations) {
                    value.updateCurtimeFrame((uint32_t)curtime);
                }
            }
            else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000/30));
            }
            lasttime = curtime;
        }
    }
#endif
};

// mininmal info about lottie aninmation, need
// for fast check we pid assigned for any animation
struct LottieAnimDesc {
    ImVec2 size;
    void *srv = nullptr;
    ImGuiID pid = BAD_PICTUREID;
};

struct LottieAnimationRenderer {
    LottieRenderThread renderThread;

    std::mutex animationsPresentMutex;
    std::unordered_map<ImGuiID, LottieAnimDesc> animationsPresent;

    ImGuiID match(const char *path, int w, int h, bool loop, int rate) {
        if (!path || 0 == *path) {
            return false;
        }

        std::lock_guard<std::mutex> lock(animationsPresentMutex);
        ImGuiID propsHash = LottieAnim::getPropsHash(path, w, h, loop, rate);
        auto it = animationsPresent.find(propsHash);
        if (it == animationsPresent.end()) {
            ImVec2 prefferedSize;
            prefferedSize.x = (float)std::max<int>(w, LottieAnim::DEFAULT_SIZE);
            prefferedSize.y = (float)std::max<int>(h, LottieAnim::DEFAULT_SIZE);
            LottieAnimDesc animDesc;
            animDesc.pid = propsHash;
            animDesc.size = prefferedSize;
            animationsPresent.insert({propsHash, animDesc});

            LottieRenderCommand command;
            command.type = LottieRenderCommand::ADD_CONFIG;
            command.path = path;
            command.w = (int)prefferedSize.x;
            command.h = (int)prefferedSize.y;
            command.loop = loop;
            command.rate = rate;
            command.pid = propsHash;
            renderThread.addCommand(command);
            return propsHash;
        }

        return propsHash;
    }

    bool render(ImGuiID pid) {
        LottieRenderCommand command;
        command.type = LottieRenderCommand::SETUP_RENDER;
        command.pid = pid;
        command.render = true;
        renderThread.addCommand(command);
        return true;
    }

    void *image(ImGuiID pid) {
        //std::lock_guard<std::mutex> lock(animationsPresentMutex);
        auto it = std::find_if(animationsPresent.begin(), animationsPresent.end(), [pid] (auto &a) { return a.second.pid == pid; });
        return (it == animationsPresent.end()) ? nullptr : it->second.srv;
    }

    void play(ImGuiID pid, bool play) {
        LottieRenderCommand command;
        command.type = LottieRenderCommand::SETUP_PLAY;
        command.pid = pid;
        command.play = play;
        renderThread.addCommand(command);
    }

    void discard(ImGuiID pid) {
        LottieRenderCommand command;
        command.type = LottieRenderCommand::DISCARD_PID;
        command.pid = pid;
        renderThread.addCommand(command);

        std::lock_guard<std::mutex> lock(animationsPresentMutex);
        auto it = std::find_if(animationsPresent.begin(), animationsPresent.end(), [pid] (auto &a) { return a.second.pid == pid; });
        if (it != animationsPresent.end()) {
            animationsPresent.erase(it);
        }
    }

#ifdef IMLOTTIE_DX11_IMPLEMENTATION
    void uploadReadyFramesToSysTex(ID3D11Device *pd3dDevice, ID3D11DeviceContext* ctx) {
        // prepared frames stored in readyFrames array now (in system memory)
        // but need move those to gpu memory with textures
        ReadyFrame readyFrame;
        while (renderThread.popReadyFrame(readyFrame)) {
            const auto &it = renderThread.animations.find(readyFrame.pid);

            if (!it->second.texture) {
                it->second.createTextureFromData(readyFrame.data.data(), pd3dDevice);
                auto rit = std::find_if(animationsPresent.begin(), animationsPresent.end(), [pid = it->second.pid] (auto &a) { return a.second.pid == pid; });
                if (rit != animationsPresent.end())
                    rit->second.srv = it->second.srv;
                break;
            } else {
                it->second.updateTextureFromData(readyFrame.data.data(), ctx);
            }
        }

        renderThread.curtime = (float)ImGui::GetTime() * 1000.f;
    }
#endif // IMLOTTIE_DX11_IMPLEMENTATION

#ifdef IMLOTTIE_OPENGL_IMPLEMENTATION
    void uploadReadyFramesToSysTex() {
        // prepared frames stored in readyFrames array now (in system memory)
        // but need move those to gpu memory with textures
        ReadyFrame readyFrame;
        while (renderThread.popReadyFrame(readyFrame)) {
            const auto &it = renderThread.animations.find(readyFrame.pid);

            if (!it->second.texture) {
                it->second.createTextureFromData(readyFrame.data.data());
                auto rit = std::find_if(animationsPresent.begin(), animationsPresent.end(), [pid = it->second.pid] (auto &a) { return a.second.pid == pid; });
                if (rit != animationsPresent.end())
                    rit->second.srv = (ImTextureID)(intptr_t)it->second.srv;
                break;
            } else {
                it->second.updateTextureFromData(readyFrame.data.data());
            }
        }

        renderThread.curtime = (float)ImGui::GetTime() * 1000.f;
    }

#ifdef IMLOTTIE_SIMPLE_IMPLEMENTATION
    void simpleUploadTex() {
        for (auto & [k, anim_] : renderThread.animations)
        {
            if (!anim_.texture) {
                anim_.createTextureFromData(anim_.currentFrame.data.data());

                auto rit = std::find_if(animationsPresent.begin(), animationsPresent.end(), [pid = anim_.pid](auto& a) { return a.second.pid == pid; });
                if (rit != animationsPresent.end())
                    rit->second.srv = (ImTextureID)(intptr_t)anim_.srv;
                break;
            }
            else {
                anim_.updateTextureFromData(anim_.currentFrame.data.data());
            }
        }
        renderThread.curtime = (float)ImGui::GetTime() * 1000.f;
    }
#endif

#endif

    LottieAnimationRenderer() : independedThread{}  {
#ifdef IMLOTTIE_SIMPLE_IMPLEMENTATION
        independedThread = std::thread([this] () { renderThread.simpleExecute(); });
#else
        independedThread = std::thread{[this] () { renderThread.execute(); }};
#endif
    }

    ~LottieAnimationRenderer() {
        renderThread.terminating.store(true);
        independedThread.join();
    }
    private:
        std::thread independedThread;
};

void LottieAnimation(const char *path, const ImVec2 &size, bool loop, int rate) {
    ImVec2 pos, centre;
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext &g = *GImGui;
    const ImGuiStyle &style = g.Style;
    const ImGuiID id = window->GetID(path);

    pos = window->DC.CursorPos;

    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(bb, style.FramePadding.y);

    centre = bb.GetCenter();
    if (!ImGui::ItemAdd(bb, id))
        return;

    assert(detail::g_lottieRenderer);
    if (detail::g_lottieRenderer) {
        ImGuiID rid = detail::g_lottieRenderer->match(path, size.x, size.y, loop, rate);
#ifndef IMLOTTIE_SIMPLE_IMPLEMENTATION
        detail::g_lottieRenderer->render(rid); // not really render, just send command to stack we need this texture
#endif
        void *texture = detail::g_lottieRenderer->image(rid); // get texture from renderer or null if not present
        window->DrawList->AddImage((void *)texture, bb.Min, bb.Max, ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)));
    } else {
        window->DrawList->AddRectFilled(bb.Min, bb.Max, 0xffffffff);
    }
}

void LottieAnimation(const char *path, const ImVec2 &animSize, const ImVec2 &size, bool loop, int rate) {
    ImVec2 pos, centre;
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext &g = *GImGui;
    const ImGuiStyle &style = g.Style;
    const ImGuiID id = window->GetID(path);

    pos = window->DC.CursorPos;

    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(bb, style.FramePadding.y);

    centre = bb.GetCenter();
    if (!ImGui::ItemAdd(bb, id))
        return;

    assert(detail::g_lottieRenderer);
    if (detail::g_lottieRenderer) {
        ImGuiID rid = detail::g_lottieRenderer->match(path, animSize.x, animSize.y, loop, rate);
#ifndef IMLOTTIE_SIMPLE_IMPLEMENTATION
        detail::g_lottieRenderer->render(rid); // not really render, just send command to stack we need this texture
#endif
        void *texture = detail::g_lottieRenderer->image(rid); // get texture from renderer or null if not present
        window->DrawList->AddImage((void *)texture, bb.Min, bb.Max, ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)));
    } else {
        window->DrawList->AddRectFilled(bb.Min, bb.Max, 0xffffffff);
    }
}


void init() {
    detail::g_lottieRenderer = new LottieAnimationRenderer();
}

void destroy() {
    delete detail::g_lottieRenderer;
    detail::g_lottieRenderer = nullptr;
}


template<typename ... Args>
void sync(Args... args) {
#ifdef IMLOTTIE_SIMPLE_IMPLEMENTATION
    if (detail::g_lottieRenderer) {
        detail::g_lottieRenderer->simpleUploadTex(args...);
    }
#else
    if (detail::g_lottieRenderer) {
        detail::g_lottieRenderer->uploadReadyFramesToSysTex(args...);
    }
#endif
}

#ifdef IMLOTTIE_DEMO
void demoAnimations(const std::string &demo_folder) {
    ImGui::Begin("Hello, Lottie!");                          // Create a window called "Hello, Lottie!" and append into it.
    ImGui::Text("This is some useful animations.");

    auto _ = [&demo_folder] (const char *anim) { return demo_folder + anim; };

    ImLottie::LottieAnimation(_("speaker.json").c_str(), ImVec2(48, 48), true, 0); ImGui::SameLine();
    ImLottie::LottieAnimation(_("cubes.json").c_str(), ImVec2(48, 48), true, 0); ImGui::SameLine();
    ImLottie::LottieAnimation(_("emojilove.json").c_str(), ImVec2(48, 48), true, 0); ImGui::SameLine();
    ImLottie::LottieAnimation(_("car.json").c_str(), ImVec2(64, 64), true, 0); ImGui::SameLine();
    ImLottie::LottieAnimation(_("seeu.json").c_str(), ImVec2(64, 64), true, 0); ImGui::SameLine();
    ImLottie::LottieAnimation(_("freeside.json").c_str(), ImVec2(64, 64), true, 0);
    //next line
    ImLottie::LottieAnimation(_("valentine.json").c_str(), ImVec2(128, 128), true, 0); ImGui::SameLine();
    ImLottie::LottieAnimation(_("jellyfish.json").c_str(), ImVec2(64, 64), true, 0); ImGui::SameLine();
    ImLottie::LottieAnimation(_("updown.json").c_str(), ImVec2(64, 64), true, 0); ImGui::SameLine();
    ImLottie::LottieAnimation(_("smarthome.json").c_str(), ImVec2(64, 64), true, 0); ImGui::SameLine();
    ImLottie::LottieAnimation(_("typing.json").c_str(), ImVec2(64, 64), true, 0);
    //next line
    ImLottie::LottieAnimation(_("explosion.json").c_str(), ImVec2(64, 64), true, 0); ImGui::SameLine();
    ImLottie::LottieAnimation(_("heart.json").c_str(), ImVec2(64, 64), true, 0); ImGui::SameLine();
    ImLottie::LottieAnimation(_("angrycloud.json").c_str(), ImVec2(64, 64), true, 0); ImGui::SameLine();
    ImLottie::LottieAnimation(_("welcome.json").c_str(), ImVec2(64, 64), true, 0); ImGui::SameLine();
    ImLottie::LottieAnimation(_("2023.json").c_str(), ImVec2(64, 64), true, 0); ImGui::SameLine();
    ImLottie::LottieAnimation(_("fly1.json").c_str(), ImVec2(64, 64), true, 0);
    //next line
    ImLottie::LottieAnimation(_("runcycle.json").c_str(), ImVec2(64, 64), true, 0); ImGui::SameLine();
    ImLottie::LottieAnimation(_("email.json").c_str(), ImVec2(64, 64), true, 0); ImGui::SameLine();
    ImLottie::LottieAnimation(_("confused.json").c_str(), ImVec2(64, 64), true, 0);

    ImGui::End();
}
#endif // IMLOTTIE_DEMO

} // end namespace ImLottie