/*
 * Thanks for Samsung Electronics for amazing rlottie library.
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

#include <cmath>
#include <cstdint>
#include <assert.h>
#include <vector>
#include <unordered_map>
#include <future>
#include <memory>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <sstream>
#include <mutex>
#include <atomic>
#include <array>
#include <bitset>
#include <deque>

#ifdef __cplusplus
extern "C" {
#endif
    unsigned char *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp);
    unsigned char *stbi_load_from_memory(unsigned char const *buffer, int len, int *x, int *y, int *channels_in_file, int desired_channels);
    void stbi_image_free(void *data);

#ifdef __cplusplus
}
#endif

namespace imlottie {

struct VDebug {
    template<typename Args>
    VDebug &operator<<(const Args &) { return *this; }
};

#define vDebug VDebug()
#define vWarning VDebug()
#define vCritical VDebug()

using uint = uint32_t;
using ushort = uint16_t;
using uchar = uint8_t;

#ifndef __has_attribute
# define __has_attribute(x) 0
#endif /* !__has_attribute */

#if __has_attribute(unused)
# define V_UNUSED __attribute__((__unused__))
#else
# define V_UNUSED
#endif /* V_UNUSED */

#if __has_attribute(warn_unused_result)
# define V_REQUIRED_RESULT __attribute__((__warn_unused_result__))
#else
# define V_REQUIRED_RESULT
#endif /* V_REQUIRED_RESULT */

#define V_CONSTEXPR constexpr
#define V_NOTHROW noexcept

#if __GNUC__ >= 7
#define VECTOR_FALLTHROUGH __attribute__ ((fallthrough));
#else
#define VECTOR_FALLTHROUGH
#endif

struct RjValue
{
    RjValue();
    ~RjValue();

    void SetNull();
    void SetBool(bool b);
    bool GetBool() const;
    void SetInt(int i);
    int GetInt() const;
    void SetUint(unsigned u);
    void SetInt64(int64_t i64);
    void SetUint64(uint64_t u64);
    void SetDouble(double d);
    double GetDouble() const;
    void SetFloat(float f);
    void SetString(const char* str,size_t length);
    const char* GetString() const;

    int GetType()  const;
    bool IsNull()   const;
    bool IsFalse()  const;
    bool IsTrue()   const;
    bool IsBool()   const;
    bool IsObject() const;
    bool IsArray()  const;
    bool IsNumber() const;
    bool IsInt()    const;
    bool IsUint()   const;
    bool IsInt64()  const;
    bool IsUint64() const;
    bool IsDouble() const;
    bool IsString() const;

    void* v_ = nullptr;
};

struct RjInsituStringStream
{
    RjInsituStringStream(char* str);
    void* ss_ = nullptr;
};

struct LookaheadParserHandlerBase
{
    virtual bool Null() = 0;
    virtual bool Bool(bool b) = 0;
    virtual bool Int(int i) = 0;
    virtual bool Uint(unsigned u) = 0;
    virtual bool Int64(int64_t i) = 0;
    virtual bool Uint64(int64_t u) = 0;
    virtual bool Double(double d) = 0;
    virtual bool RawNumber(const char *, unsigned length, bool) = 0;
    virtual bool String(const char *str, unsigned length, bool) = 0;
    virtual bool StartObject() = 0;
    virtual bool Key(const char *str, unsigned length, bool) = 0;
    virtual bool EndObject(unsigned) = 0;
    virtual bool StartArray() = 0;
    virtual bool EndArray(unsigned) = 0;
};

struct RjReader
{
    RjReader();

    void IterativeParseInit();
    bool HasParseError() const;
    bool IterativeParseNext(int parseFlags, RjInsituStringStream& ss_, LookaheadParserHandlerBase& handler);

    void* r_ = nullptr;
};


template <typename T>
V_CONSTEXPR inline const T &vMin(const T &a, const T &b) {
    return (a < b) ? a : b;
}
template <typename T>
V_CONSTEXPR inline const T &vMax(const T &a, const T &b) {
    return (a < b) ? b : a;
}

static const double EPSILON_DOUBLE = 0.000000000001f;
static const float  EPSILON_FLOAT = 0.000001f;

static inline bool vCompare(float p1, float p2) {
    return (std::abs(p1 - p2) < EPSILON_FLOAT);
}

static inline bool vIsZero(float f) {
    return (std::abs(f) <= EPSILON_FLOAT);
}

static inline bool vIsZero(double f) {
    return (std::abs(f) <= EPSILON_DOUBLE);
}

class vFlagHelper {
    int i;

public:
    explicit constexpr inline vFlagHelper(int ai) noexcept : i(ai) {}
    constexpr inline operator int() const noexcept { return i; }

    explicit constexpr inline vFlagHelper(uint ai) noexcept : i(int(ai)) {}
    explicit constexpr inline vFlagHelper(short ai) noexcept : i(int(ai)) {}
    explicit constexpr inline vFlagHelper(ushort ai) noexcept : i(int(uint(ai))) {}
    constexpr inline operator uint() const noexcept { return uint(i); }
};

struct Color {
    Color() = default;
    Color(float r, float g , float b):_r(r), _g(g), _b(b){}
    float r() const {return _r;}
    float g() const {return _g;}
    float b() const {return _b;}
private:
    float _r{0};
    float _g{0};
    float _b{0};
};

struct Size {
    Size() = default;
    Size(float w, float h):_w(w), _h(h){}
    float w() const {return _w;}
    float h() const {return _h;}
private:
    float _w{0};
    float _h{0};
};

struct Point {
    Point() = default;
    Point(float x, float y):_x(x), _y(y){}
    float x() const {return _x;}
    float y() const {return _y;}
private:
    float _x{0};
    float _y{0};
};

enum LOTMaskType: unsigned char
{
    MaskAdd = 0,
    MaskSubstract,
    MaskIntersect,
    MaskDifference
};

enum LOTMatteType: uchar
{
    MatteNone = 0,
    MatteAlpha,
    MatteAlphaInv,
    MatteLuma,
    MatteLumaInv
};

struct LOTMask {
    struct {
        const float *ptPtr;
        size_t       ptCount;
        const char*  elmPtr;
        size_t       elmCount;
    } mPath;
    LOTMaskType mMode;
    unsigned char mAlpha;
};
struct LOTNode;
struct LOTLayerNode {
    struct {
        LOTMask        *ptr;
        size_t          size;
    } mMaskList;

    struct {
        const float *ptPtr;
        size_t       ptCount;
        const char  *elmPtr;
        size_t       elmCount;
    } mClipPath;

    struct {
        struct LOTLayerNode   **ptr;
        size_t                  size;
    } mLayerList;

    struct {
        LOTNode        **ptr;
        size_t           size;
    } mNodeList;

    LOTMatteType mMatte;
    int          mVisible;
    unsigned char mAlpha;
    const char  *keypath;

};

template <typename Enum>
class vFlag {
public:
    static_assert(
        (sizeof(Enum) <= sizeof(int)),
        "vFlag only supports int as storage so bigger type will overflow");
    static_assert((std::is_enum<Enum>::value),
                  "vFlag is only usable on enumeration types.");

    using Int = typename std::conditional <
                std::is_unsigned<typename std::underlying_type<Enum>::type>::value,
                unsigned int, signed int >::type;

    using  enum_type = Enum;
    // compiler-generated copy/move ctor/assignment operators are fine!

    vFlag() = default;
    constexpr vFlag(Enum f) noexcept : i(Int(f)) {}
    explicit constexpr vFlag(vFlagHelper f) noexcept : i(f) {}

    inline vFlag &operator&=(int mask) noexcept {
        i &= mask;
        return *this;
    }
    inline vFlag &operator&=(uint mask) noexcept {
        i &= mask;
        return *this;
    }
    inline vFlag &operator&=(Enum mask) noexcept {
        i &= Int(mask);
        return *this;
    }
    inline vFlag &operator|=(vFlag f) noexcept {
        i |= f.i;
        return *this;
    }
    inline vFlag &operator|=(Enum f) noexcept {
        i |= Int(f);
        return *this;
    }
    inline vFlag &operator^=(vFlag f) noexcept {
        i ^= f.i;
        return *this;
    }
    inline vFlag &operator^=(Enum f) noexcept {
        i ^= Int(f);
        return *this;
    }

    constexpr inline operator Int() const noexcept { return i; }

    constexpr inline vFlag operator|(vFlag f) const {
        return vFlag(vFlagHelper(i | f.i));
    }
    constexpr inline vFlag operator|(Enum f) const noexcept {
        return vFlag(vFlagHelper(i | Int(f)));
    }
    constexpr inline vFlag operator^(vFlag f) const noexcept {
        return vFlag(vFlagHelper(i ^ f.i));
    }
    constexpr inline vFlag operator^(Enum f) const noexcept {
        return vFlag(vFlagHelper(i ^ Int(f)));
    }
    constexpr inline vFlag operator&(int mask) const noexcept {
        return vFlag(vFlagHelper(i & mask));
    }
    constexpr inline vFlag operator&(uint mask) const noexcept {
        return vFlag(vFlagHelper(i & mask));
    }
    constexpr inline vFlag operator&(Enum f) const noexcept {
        return vFlag(vFlagHelper(i & Int(f)));
    }
    constexpr inline vFlag operator~() const noexcept {
        return vFlag(vFlagHelper(~i));
    }

    constexpr inline bool operator!() const noexcept { return !i; }

    constexpr inline bool testFlag(Enum f) const noexcept {
        return (i & Int(f)) == Int(f) && (Int(f) != 0 || i == Int(f));
    }
    inline vFlag &setFlag(Enum f, bool on = true) noexcept {
        return on ? (*this |= f) : (*this &= ~f);
    }

    Int i{0};
};

class VColor {
public:
    VColor() = default;
    explicit VColor(uchar red, uchar green, uchar blue, uchar alpha = 255) noexcept
        : a(alpha), r(red), g(green), b(blue) {}
    inline uchar  red() const noexcept { return r; }
    inline uchar  green() const noexcept { return g; }
    inline uchar  blue() const noexcept { return b; }
    inline uchar  alpha() const noexcept { return a; }
    inline void setRed(uchar red) noexcept { r = red; }
    inline void setGreen(uchar green) noexcept { g = green; }
    inline void setBlue(uchar blue) noexcept { b = blue; }
    inline void setAlpha(uchar alpha) noexcept { a = alpha; }
    inline bool isOpaque() const { return a == 255; }
    inline bool isTransparent() const { return a == 0; }
    inline bool operator==(const VColor &o) const {
        return ((a == o.a) && (r == o.r) && (g == o.g) && (b == o.b));
    }
    uint premulARGB() const {
        int pr = (r * a) / 255;
        int pg = (g * a) / 255;
        int pb = (b * a) / 255;
        return uint((a << 24) | (pr << 16) | (pg << 8) | (pb));
    }

    uint premulARGB(float opacity) const {
        int alpha = int(a * opacity);
        int pr = (r * alpha) / 255;
        int pg = (g * alpha) / 255;
        int pb = (b * alpha) / 255;
        return uint((alpha << 24) | (pr << 16) | (pg << 8) | (pb));
    }

public:
    uchar a{0};
    uchar r{0};
    uchar g{0};
    uchar b{0};
};

enum class FillRule : unsigned char { EvenOdd, Winding };
enum class JoinStyle : unsigned char { Miter, Bevel, Round };
enum class CapStyle : unsigned char { Flat, Square, Round };

enum class BlendMode {
    Src,
    SrcOver,
    DestIn,
    DestOut
};

#ifndef V_CONSTRUCTOR_FUNCTION
#define V_CONSTRUCTOR_FUNCTION0(AFUNC)            \
    namespace {                                   \
    static const struct AFUNC##_ctor_class_ {     \
        inline AFUNC##_ctor_class_() { AFUNC(); } \
    } AFUNC##_ctor_instance_;                     \
    }

#define V_CONSTRUCTOR_FUNCTION(AFUNC) V_CONSTRUCTOR_FUNCTION0(AFUNC)
#endif

class VPointF {
public:
    VPointF() = default;
    constexpr inline VPointF(float x, float y) noexcept : mx(x), my(y) {}
    constexpr inline float x() const noexcept { return mx; }
    constexpr inline float y() const noexcept { return my; }
    inline float &rx() noexcept { return mx; }
    inline float &ry() noexcept { return my; }
    inline void            setX(float x) { mx = x; }
    inline void            setY(float y) { my = y; }
    inline VPointF         operator-() noexcept { return { -mx, -my}; }
    inline VPointF &operator+=(const VPointF &p) noexcept {
        mx += p.mx;
        my += p.my;
        return *this;
    }
    inline VPointF &operator-=(const VPointF &p) noexcept {
        mx -= p.mx;
        my -= p.my;
        return *this;
    }
    friend const VPointF   operator+(const VPointF &p1, const VPointF &p2) {
        return VPointF(p1.mx + p2.mx, p1.my + p2.my);
    }
    inline friend bool fuzzyCompare(const VPointF &p1, const VPointF &p2);

    friend inline VPointF       operator-(const VPointF &p1, const VPointF &p2);
    friend inline const VPointF operator*(const VPointF &, float);
    friend inline const VPointF operator*(float, const VPointF &);
    friend inline const VPointF operator/(const VPointF &, float);
    friend inline const VPointF operator/(float, const VPointF &);

private:
    float mx{0};
    float my{0};
};

inline bool fuzzyCompare(const VPointF &p1, const VPointF &p2) { return (vCompare(p1.mx, p2.mx) && vCompare(p1.my, p2.my)); }
inline VPointF operator-(const VPointF &p1, const VPointF &p2) { return {p1.mx - p2.mx, p1.my - p2.my}; }
inline const VPointF operator*(const VPointF &p, float c) { return VPointF(p.mx * c, p.my * c); }
inline const VPointF operator*(float c, const VPointF &p) { return VPointF(p.mx * c, p.my * c); }
inline const VPointF operator/(const VPointF &p, float c) { return VPointF(p.mx / c, p.my / c); }
inline const VPointF operator/(float c, const VPointF &p) { return VPointF(p.mx / c, p.my / c); }
inline VDebug &operator<<(VDebug &os, const VPointF &o) { os << "{P " << o.x() << "," << o.y() << "}"; return os; }

class VPoint {
public:
    VPoint() = default;
    constexpr inline VPoint(int x, int y) noexcept : mx(x), my(y) {}
    constexpr inline int  x() const noexcept { return mx; }
    constexpr inline int  y() const noexcept { return my; }
    inline void           setX(int x) { mx = x; }
    inline void           setY(int y) { my = y; }
    inline VPoint &operator+=(const VPoint &p) noexcept {
        mx += p.mx;
        my += p.my;
        return *this;
    }

    inline VPoint &operator-=(const VPoint &p) noexcept {
        mx -= p.mx;
        my -= p.my;
        return *this;
    }
    constexpr inline bool operator==(const VPoint &o) const { return (mx == o.x() && my == o.y()); }

    constexpr inline bool operator!=(const VPoint &o) const {
        return !(operator==(o));
    }
    friend inline VPoint  operator-(const VPoint &p1, const VPoint &p2);
    inline friend VDebug &operator<<(VDebug &os, const VPoint &o);

private:
    int mx{0};
    int my{0};
};
inline VDebug &operator<<(VDebug &os, const VPoint &o) { os << "{P " << o.x() << "," << o.y() << "}"; return os; }
inline VPoint operator-(const VPoint &p1, const VPoint &p2) { return {p1.mx - p2.mx, p1.my - p2.my}; }

class VLine {
public:
    VLine() = default;
    VLine(float x1, float y1, float x2, float y2)
        : mX1(x1), mY1(y1), mX2(x2), mY2(y2)     {    }
    VLine(const VPointF &p1, const VPointF &p2)
        : mX1(p1.x()), mY1(p1.y()), mX2(p2.x()), mY2(p2.y())     {    }
    float   length() const { return length(mX1, mY1, mX2, mY2); }
    void    splitAtLength(float lengthAt, VLine &left, VLine &right) const {
        float  len = length();
        float dx = ((mX2 - mX1) / len) * lengthAt;
        float dy = ((mY2 - mY1) / len) * lengthAt;

        left.mX1 = mX1;
        left.mY1 = mY1;
        left.mX2 = left.mX1 + dx;
        left.mY2 = left.mY1 + dy;

        right.mX1 = left.mX2;
        right.mY1 = left.mY2;
        right.mX2 = mX2;
        right.mY2 = mY2;
    }
    VPointF p1() const { return {mX1, mY1}; }
    VPointF p2() const { return {mX2, mY2}; }
    float angle() const {
        static constexpr float K_PI = 3.141592f;
        const float dx = mX2 - mX1;
        const float dy = mY2 - mY1;

        const float theta = std::atan2(dy, dx) * 180.0f / K_PI;
        return theta;
    }
    // approximate sqrt(x*x + y*y) using alpha max plus beta min algorithm.
    // With alpha = 1, beta = 3/8, giving results with the largest error less
    // than 7% compared to the exact value.
    static float length(float x1, float y1, float x2, float y2) {
        float x = x2 - x1;
        float y = y2 - y1;

        x = x < 0 ? -x : x;
        y = y < 0 ? -y : y;

        return (x > y ? x + 0.375f * y : y + 0.375f * x);
    }

private:
    float mX1{0};
    float mY1{0};
    float mX2{0};
    float mY2{0};
};

class VSize {
public:
    VSize() = default;
    constexpr inline VSize(int w, int h) noexcept : mw(w), mh(h) {}
    bool empty() const { return (mw <= 0 || mh <= 0); }
    constexpr inline int  width() const noexcept { return mw; }
    constexpr inline int  height() const noexcept { return mh; }
    inline void           setWidth(int w) { mw = w; }
    inline void           setHeight(int h) { mh = h; }
    inline VSize &operator+=(const VSize &p) noexcept {
        mw += p.mw;
        mh += p.mh;
        return *this;
    }

    inline VSize &operator-=(const VSize &p) noexcept {
        mw -= p.mw;
        mh -= p.mh;
        return *this;
    }

    constexpr inline bool operator==(const VSize &o) const { return (mw == o.width() && mh == o.height()); }
    constexpr inline bool operator!=(const VSize &o) const { return !(operator==(o)); }
    inline friend VDebug &operator<<(VDebug &os, const VSize &o);

private:
    int mw{0};
    int mh{0};
};
inline VDebug &operator<<(VDebug &os, const VSize &o) { os << "{P " << o.width() << "," << o.height() << "}"; return os; }

class VRectF;

class VRect {
public:
    VRect() = default;
    VRect(int x, int y, int w, int h) : x1(x), y1(y), x2(x + w), y2(y + h) {}
    explicit VRect(VPoint pt, VSize sz) : VRect(pt.x(), pt.y(), sz.width(), sz.height()) {}
    operator VRectF() const;
    V_CONSTEXPR bool empty() const { return x1 >= x2 || y1 >= y2; }
    V_CONSTEXPR int left() const { return x1; }
    V_CONSTEXPR int top() const { return y1; }
    V_CONSTEXPR int right() const { return x2; }
    V_CONSTEXPR int bottom() const { return y2; }
    V_CONSTEXPR int width() const { return x2 - x1; }
    V_CONSTEXPR int height() const { return y2 - y1; }
    V_CONSTEXPR int x() const { return x1; }
    V_CONSTEXPR int y() const { return y1; }
    VSize           size() const { return {width(), height()}; }
    void            setLeft(int l) { x1 = l; }
    void            setTop(int t) { y1 = t; }
    void            setRight(int r) { x2 = r; }
    void            setBottom(int b) { y2 = b; }
    void            setWidth(int w) { x2 = x1 + w; }
    void            setHeight(int h) { y2 = y1 + h; }
    VRect    translated(int dx, int dy) const { return {x1 + dx, y1 + dy, x2 - x1, y2 - y1}; }
    void     translate(int dx, int dy) { x1 += dx; y1 += dy; x2 += dx; y2 += dy; }
    bool     contains(const VRect &r, bool proper = false) const {
        return proper ?
               ((x1 < r.x1) && (x2 > r.x2) && (y1 < r.y1) && (y2 > r.y2)) :
               ((x1 <= r.x1) && (x2 >= r.x2) && (y1 <= r.y1) && (y2 >= r.y2));
    }
    bool     intersects(const VRect &r) { return (right() > r.left() && left() < r.right() && bottom() > r.top() && top() < r.bottom()); }
    friend V_CONSTEXPR inline bool operator==(const VRect &,
            const VRect &) noexcept;
    friend V_CONSTEXPR inline bool operator!=(const VRect &,
            const VRect &) noexcept;
    friend VDebug &operator<<(VDebug &os, const VRect &o);

    VRect intersected(const VRect &r) const { return *this & r; }
    VRect operator&(const VRect &r) const {
        if (empty()) return VRect();

        int l1 = x1;
        int r1 = x1;
        if (x2 - x1 + 1 < 0)
            l1 = x2;
        else
            r1 = x2;

        int l2 = r.x1;
        int r2 = r.x1;
        if (r.x2 - r.x1 + 1 < 0)
            l2 = r.x2;
        else
            r2 = r.x2;

        if (l1 > r2 || l2 > r1) return VRect();

        int t1 = y1;
        int b1 = y1;
        if (y2 - y1 + 1 < 0)
            t1 = y2;
        else
            b1 = y2;

        int t2 = r.y1;
        int b2 = r.y1;
        if (r.y2 - r.y1 + 1 < 0)
            t2 = r.y2;
        else
            b2 = r.y2;

        if (t1 > b2 || t2 > b1) return VRect();

        VRect tmp;
        tmp.x1 = std::max(l1, l2);
        tmp.x2 = std::min(r1, r2);
        tmp.y1 = std::max(t1, t2);
        tmp.y2 = std::min(b1, b2);
        return tmp;
    }

private:
    int x1{0};
    int y1{0};
    int x2{0};
    int y2{0};
};

inline VDebug &operator<<(VDebug &os, const VRect &o) { os << "{R " << o.x() << "," << o.y() << "," << o.width() << "," << o.height() << "}"; return os; }
V_CONSTEXPR inline bool operator==(const VRect &r1, const VRect &r2) noexcept { return r1.x1 == r2.x1 && r1.x2 == r2.x2 && r1.y1 == r2.y1 && r1.y2 == r2.y2; }
V_CONSTEXPR inline bool operator!=(const VRect &r1, const VRect &r2) noexcept { return r1.x1 != r2.x1 || r1.x2 != r2.x2 || r1.y1 != r2.y1 || r1.y2 != r2.y2; }

class VRectF {
public:
    VRectF() = default;

    VRectF(double x, double y, double w, double h) :
        x1(float(x)), y1(float(y)),
        x2(float(x + w)), y2(float(y + h)) {}
    operator VRect() const {
        return {int(left()), int(right()), int(width()), int(height())};
    }

    V_CONSTEXPR bool  empty() const { return x1 >= x2 || y1 >= y2; }
    V_CONSTEXPR float left() const { return x1; }
    V_CONSTEXPR float top() const { return y1; }
    V_CONSTEXPR float right() const { return x2; }
    V_CONSTEXPR float bottom() const { return y2; }
    V_CONSTEXPR float width() const { return x2 - x1; }
    V_CONSTEXPR float height() const { return y2 - y1; }
    V_CONSTEXPR float x() const { return x1; }
    V_CONSTEXPR float y() const { return y1; }
    V_CONSTEXPR inline VPointF center() const {
        return {x1 + (x2 - x1) / 2.f, y1 + (y2 - y1) / 2.f};
    }
    void setLeft(float l) { x1 = l; }
    void setTop(float t) { y1 = t; }
    void setRight(float r) { x2 = r; }
    void setBottom(float b) { y2 = b; }
    void setWidth(float w) { x2 = x1 + w; }
    void setHeight(float h) { y2 = y1 + h; }
    void translate(float dx, float dy) {
        x1 += dx;
        y1 += dy;
        x2 += dx;
        y2 += dy;
    }

private:
    float x1{0};
    float y1{0};
    float x2{0};
    float y2{0};
};

inline VRect::operator VRectF() const { return {double(left()), double(right()), double(width()), double(height())}; }

constexpr float deg2rad = float(0.017453292519943295769);  // pi/180
constexpr float inv_dist_to_plane = 1. / 1024.;

#define V_NEAR_CLIP 0.000001f
#ifdef TRANSMAP
#undef TRANSMAP
#endif

#define TRANSMAP(x, y, nx, ny)                           \
    do {                                                 \
        float FX_ = float(x);                            \
        float FY_ = float(y);                            \
        switch (t) {                                     \
        case MatrixType::None:                           \
            nx = FX_;                                    \
            ny = FY_;                                    \
            break;                                       \
        case MatrixType::Translate:                      \
            nx = FX_ + mtx;                              \
            ny = FY_ + mty;                              \
            break;                                       \
        case MatrixType::Scale:                          \
            nx = m11 * FX_ + mtx;                        \
            ny = m22 * FY_ + mty;                        \
            break;                                       \
        case MatrixType::Rotate:                         \
        case MatrixType::Shear:                          \
        case MatrixType::Project:                        \
            nx = m11 * FX_ + m21 * FY_ + mtx;            \
            ny = m12 * FX_ + m22 * FY_ + mty;            \
            if (t == MatrixType::Project) {              \
                float w = (m13 * FX_ + m23 * FY_ + m33); \
                if (w < V_NEAR_CLIP) w = V_NEAR_CLIP;    \
                w = 1.f / w;                              \
                nx *= w;                                 \
                ny *= w;                                 \
            }                                            \
        }                                                \
    } while (0)

struct VMatrixData;
class VMatrix {
public:
    enum class Axis { X, Y, Z };
    enum class MatrixType : unsigned char {
        None = 0x00,
        Translate = 0x01,
        Scale = 0x02,
        Rotate = 0x04,
        Shear = 0x08,
        Project = 0x10
    };
    VMatrix() = default;
    bool         isAffine() const {
        return type() < MatrixType::Project;
    }
    bool         isIdentity() const {
        return type() == MatrixType::None;
    }
    bool         isInvertible() const {
        return !vIsZero(determinant());
    }
    bool         isScaling() const {
        return type() >= MatrixType::Scale;
    }
    bool         isRotating() const {
        return type() >= MatrixType::Rotate;
    }
    bool         isTranslating() const {
        return type() >= MatrixType::Translate;
    }
    MatrixType   type() const {
        if (dirty == MatrixType::None || dirty < mType) return mType;

        switch (dirty) {
        case MatrixType::Project:
            if (!vIsZero(m13) || !vIsZero(m23) || !vIsZero(m33 - 1)) {
                mType = MatrixType::Project;
                break;
            }
            VECTOR_FALLTHROUGH
        case MatrixType::Shear:
        case MatrixType::Rotate:
            if (!vIsZero(m12) || !vIsZero(m21)) {
                const float dot = m11 * m12 + m21 * m22;
                if (vIsZero(dot))
                    mType = MatrixType::Rotate;
                else
                    mType = MatrixType::Shear;
                break;
            }
            VECTOR_FALLTHROUGH
        case MatrixType::Scale:
            if (!vIsZero(m11 - 1) || !vIsZero(m22 - 1)) {
                mType = MatrixType::Scale;
                break;
            }
            VECTOR_FALLTHROUGH
        case MatrixType::Translate:
            if (!vIsZero(mtx) || !vIsZero(mty)) {
                mType = MatrixType::Translate;
                break;
            }
            VECTOR_FALLTHROUGH
        case MatrixType::None:
            mType = MatrixType::None;
            break;
        }

        dirty = MatrixType::None;
        return mType;
    }
    inline float determinant() const {
        return m11 * (m33 * m22 - mty * m23) - m21 * (m33 * m12 - mty * m13) +
               mtx * (m23 * m12 - m22 * m13);
    }

    float        m_11() const { return m11; }
    float        m_12() const { return m12; }
    float        m_13() const { return m13; }

    float        m_21() const { return m21; }
    float        m_22() const { return m22; }
    float        m_23() const { return m23; }

    float        m_tx() const { return mtx; }
    float        m_ty() const { return mty; }
    float        m_33() const { return m33; }

    VMatrix &translate(VPointF pos) { return translate(pos.x(), pos.y()); }
    VMatrix &translate(float dx, float dy) {
        if (dx == 0 && dy == 0) return *this;

        switch (type()) {
        case MatrixType::None:
            mtx = dx;
            mty = dy;
            break;
        case MatrixType::Translate:
            mtx += dx;
            mty += dy;
            break;
        case MatrixType::Scale:
            mtx += dx * m11;
            mty += dy * m22;
            break;
        case MatrixType::Project:
            m33 += dx * m13 + dy * m23;
            VECTOR_FALLTHROUGH
        case MatrixType::Shear:
        case MatrixType::Rotate:
            mtx += dx * m11 + dy * m21;
            mty += dy * m22 + dx * m12;
            break;
        }
        if (dirty < MatrixType::Translate) dirty = MatrixType::Translate;
        return *this;
    }
    VMatrix &scale(VPointF s) { return scale(s.x(), s.y()); }
    VMatrix &scale(float sx, float sy) {
        if (sx == 1 && sy == 1) return *this;

        switch (type()) {
        case MatrixType::None:
        case MatrixType::Translate:
            m11 = sx;
            m22 = sy;
            break;
        case MatrixType::Project:
            m13 *= sx;
            m23 *= sy;
            VECTOR_FALLTHROUGH
        case MatrixType::Rotate:
        case MatrixType::Shear:
            m12 *= sx;
            m21 *= sy;
            VECTOR_FALLTHROUGH
        case MatrixType::Scale:
            m11 *= sx;
            m22 *= sy;
            break;
        }
        if (dirty < MatrixType::Scale) dirty = MatrixType::Scale;
        return *this;
    }

    VMatrix &shear(float sh, float sv) {
        if (sh == 0 && sv == 0) return *this;

        switch (type()) {
        case MatrixType::None:
        case MatrixType::Translate:
            m12 = sv;
            m21 = sh;
            break;
        case MatrixType::Scale:
            m12 = sv * m22;
            m21 = sh * m11;
            break;
        case MatrixType::Project:
        {
            float tm13 = sv * m23;
            float tm23 = sh * m13;
            m13 += tm13;
            m23 += tm23;
            VECTOR_FALLTHROUGH
        }
        case MatrixType::Rotate:
        case MatrixType::Shear:
        {
            float tm11 = sv * m21;
            float tm22 = sh * m12;
            float tm12 = sv * m22;
            float tm21 = sh * m11;
            m11 += tm11;
            m12 += tm12;
            m21 += tm21;
            m22 += tm22;
            break;
        }
        }
        if (dirty < MatrixType::Shear) dirty = MatrixType::Shear;
        return *this;
    }
    VMatrix &rotate(float a, Axis axis = VMatrix::Axis::Z) {
        if (a == 0) return *this;

        float sina = 0;
        float cosa = 0;
        if (a == 90. || a == -270.)
            sina = 1.;
        else if (a == 270. || a == -90.)
            sina = -1.;
        else if (a == 180.)
            cosa = -1.;
        else {
            float b = deg2rad * a;  // convert to radians
            sina = std::sin(b);     // fast and convenient
            cosa = std::cos(b);
        }

        if (axis == Axis::Z) {
            switch (type()) {
            case MatrixType::None:
            case MatrixType::Translate:
                m11 = cosa;
                m12 = sina;
                m21 = -sina;
                m22 = cosa;
                break;
            case MatrixType::Scale:
            {
                float tm11 = cosa * m11;
                float tm12 = sina * m22;
                float tm21 = -sina * m11;
                float tm22 = cosa * m22;
                m11 = tm11;
                m12 = tm12;
                m21 = tm21;
                m22 = tm22;
                break;
            }
            case MatrixType::Project:
            {
                float tm13 = cosa * m13 + sina * m23;
                float tm23 = -sina * m13 + cosa * m23;
                m13 = tm13;
                m23 = tm23;
                VECTOR_FALLTHROUGH
            }
            case MatrixType::Rotate:
            case MatrixType::Shear:
            {
                float tm11 = cosa * m11 + sina * m21;
                float tm12 = cosa * m12 + sina * m22;
                float tm21 = -sina * m11 + cosa * m21;
                float tm22 = -sina * m12 + cosa * m22;
                m11 = tm11;
                m12 = tm12;
                m21 = tm21;
                m22 = tm22;
                break;
            }
            }
            if (dirty < MatrixType::Rotate) dirty = MatrixType::Rotate;
        } else {
            VMatrix result;
            if (axis == Axis::Y) {
                result.m11 = cosa;
                result.m13 = -sina * inv_dist_to_plane;
            } else {
                result.m22 = cosa;
                result.m23 = -sina * inv_dist_to_plane;
            }
            result.mType = MatrixType::Project;
            *this = result * *this;
        }

        return *this;
    }
    VMatrix &rotateRadians(float a, Axis axis = VMatrix::Axis::Z);

    VPointF        map(const VPointF &p) const {
        float fx = p.x();
        float fy = p.y();

        float x = 0, y = 0;

        VMatrix::MatrixType t = type();
        switch (t) {
        case MatrixType::None:
            x = fx;
            y = fy;
            break;
        case MatrixType::Translate:
            x = fx + mtx;
            y = fy + mty;
            break;
        case MatrixType::Scale:
            x = m11 * fx + mtx;
            y = m22 * fy + mty;
            break;
        case MatrixType::Rotate:
        case MatrixType::Shear:
        case MatrixType::Project:
            x = m11 * fx + m21 * fy + mtx;
            y = m12 * fx + m22 * fy + mty;
            if (t == MatrixType::Project) {
                float w = 1.0f / (m13 * fx + m23 * fy + m33);
                x *= w;
                y *= w;
            }
        }
        return {x, y};
    }
    inline VPointF map(float x, float y) const { return map(VPointF(x, y)); }
    VRect          map(const VRect &rect) const {
        VMatrix::MatrixType t = type();
        if (t <= MatrixType::Translate)
            return rect.translated(std::lround(mtx), std::lround(mty));

        if (t <= MatrixType::Scale) {
            int x = std::lround(m11 * rect.x() + mtx);
            int y = std::lround(m22 * rect.y() + mty);
            int w = std::lround(m11 * rect.width());
            int h = std::lround(m22 * rect.height());
            if (w < 0) {
                w = -w;
                x -= w;
            }
            if (h < 0) {
                h = -h;
                y -= h;
            }
            return {x, y, w, h};
        } else if (t < MatrixType::Project) {
            // see mapToPolygon for explanations of the algorithm.
            float x = 0.f, y = 0.f;
            TRANSMAP(rect.left(), rect.top(), x, y);
            float xmin = x;
            float ymin = y;
            float xmax = x;
            float ymax = y;
            TRANSMAP(rect.right() + 1, rect.top(), x, y);
            xmin = vMin(xmin, x);
            ymin = vMin(ymin, y);
            xmax = vMax(xmax, x);
            ymax = vMax(ymax, y);
            TRANSMAP(rect.right() + 1, rect.bottom() + 1, x, y);
            xmin = vMin(xmin, x);
            ymin = vMin(ymin, y);
            xmax = vMax(xmax, x);
            ymax = vMax(ymax, y);
            TRANSMAP(rect.left(), rect.bottom() + 1, x, y);
            xmin = vMin(xmin, x);
            ymin = vMin(ymin, y);
            xmax = vMax(xmax, x);
            ymax = vMax(ymax, y);
            return VRect(std::lround(xmin), std::lround(ymin),
                         std::lround(xmax) - std::lround(xmin),
                         std::lround(ymax) - std::lround(ymin));
        } else {
            // Not supported
            assert(0);
            return {};
        }
    }

    V_REQUIRED_RESULT VMatrix inverted(bool *invertible = nullptr) const {
        VMatrix invert;
        bool    inv = true;

        switch (type()) {
        case MatrixType::None:
            break;
        case MatrixType::Translate:
            invert.mtx = -mtx;
            invert.mty = -mty;
            break;
        case MatrixType::Scale:
            inv = !vIsZero(m11);
            inv &= !vIsZero(m22);
            if (inv) {
                invert.m11 = 1.0f / m11;
                invert.m22 = 1.0f / m22;
                invert.mtx = -mtx * invert.m11;
                invert.mty = -mty * invert.m22;
            }
            break;
        default:
            // general case
            float det = determinant();
            inv = !vIsZero(det);
            if (inv) invert = (adjoint() /= det);
            // TODO Test above line
            break;
        }

        if (invertible) *invertible = inv;

        if (inv) {
            // inverting doesn't change the type
            invert.mType = mType;
            invert.dirty = dirty;
        }

        return invert;
    }
    V_REQUIRED_RESULT VMatrix adjoint() const {
        float h11, h12, h13, h21, h22, h23, h31, h32, h33;
        h11 = m22 * m33 - m23 * mty;
        h21 = m23 * mtx - m21 * m33;
        h31 = m21 * mty - m22 * mtx;
        h12 = m13 * mty - m12 * m33;
        h22 = m11 * m33 - m13 * mtx;
        h32 = m12 * mtx - m11 * mty;
        h13 = m12 * m23 - m13 * m22;
        h23 = m13 * m21 - m11 * m23;
        h33 = m11 * m22 - m12 * m21;

        VMatrix res;
        res.m11 = h11;
        res.m12 = h12;
        res.m13 = h13;
        res.m21 = h21;
        res.m22 = h22;
        res.m23 = h23;
        res.mtx = h31;
        res.mty = h32;
        res.m33 = h33;
        res.mType = MatrixType::None;
        res.dirty = MatrixType::Project;

        return res;
    }

    VMatrix              operator*(const VMatrix &m) const {
        const MatrixType otherType = m.type();
        if (otherType == MatrixType::None) return *this;

        const MatrixType thisType = type();
        if (thisType == MatrixType::None) return m;

        VMatrix    t;
        MatrixType type = vMax(thisType, otherType);
        switch (type) {
        case MatrixType::None:
            break;
        case MatrixType::Translate:
            t.mtx = mtx + m.mtx;
            t.mty += mty + m.mty;
            break;
        case MatrixType::Scale:
        {
            float m11v = m11 * m.m11;
            float m22v = m22 * m.m22;

            float m31v = mtx * m.m11 + m.mtx;
            float m32v = mty * m.m22 + m.mty;

            t.m11 = m11v;
            t.m22 = m22v;
            t.mtx = m31v;
            t.mty = m32v;
            break;
        }
        case MatrixType::Rotate:
        case MatrixType::Shear:
        {
            float m11v = m11 * m.m11 + m12 * m.m21;
            float m12v = m11 * m.m12 + m12 * m.m22;

            float m21v = m21 * m.m11 + m22 * m.m21;
            float m22v = m21 * m.m12 + m22 * m.m22;

            float m31v = mtx * m.m11 + mty * m.m21 + m.mtx;
            float m32v = mtx * m.m12 + mty * m.m22 + m.mty;

            t.m11 = m11v;
            t.m12 = m12v;
            t.m21 = m21v;
            t.m22 = m22v;
            t.mtx = m31v;
            t.mty = m32v;
            break;
        }
        case MatrixType::Project:
        {
            float m11v = m11 * m.m11 + m12 * m.m21 + m13 * m.mtx;
            float m12v = m11 * m.m12 + m12 * m.m22 + m13 * m.mty;
            float m13v = m11 * m.m13 + m12 * m.m23 + m13 * m.m33;

            float m21v = m21 * m.m11 + m22 * m.m21 + m23 * m.mtx;
            float m22v = m21 * m.m12 + m22 * m.m22 + m23 * m.mty;
            float m23v = m21 * m.m13 + m22 * m.m23 + m23 * m.m33;

            float m31v = mtx * m.m11 + mty * m.m21 + m33 * m.mtx;
            float m32v = mtx * m.m12 + mty * m.m22 + m33 * m.mty;
            float m33v = mtx * m.m13 + mty * m.m23 + m33 * m.m33;

            t.m11 = m11v;
            t.m12 = m12v;
            t.m13 = m13v;
            t.m21 = m21v;
            t.m22 = m22v;
            t.m23 = m23v;
            t.mtx = m31v;
            t.mty = m32v;
            t.m33 = m33v;
        }
        }

        t.dirty = type;
        t.mType = type;

        return t;
    }
    VMatrix &operator*=(const VMatrix &o)     {
        const MatrixType otherType = o.type();
        if (otherType == MatrixType::None) return *this;

        const MatrixType thisType = type();
        if (thisType == MatrixType::None) return operator=(o);

        MatrixType t = vMax(thisType, otherType);
        switch (t) {
        case MatrixType::None:
            break;
        case MatrixType::Translate:
            mtx += o.mtx;
            mty += o.mty;
            break;
        case MatrixType::Scale:
        {
            float m11v = m11 * o.m11;
            float m22v = m22 * o.m22;

            float m31v = mtx * o.m11 + o.mtx;
            float m32v = mty * o.m22 + o.mty;

            m11 = m11v;
            m22 = m22v;
            mtx = m31v;
            mty = m32v;
            break;
        }
        case MatrixType::Rotate:
        case MatrixType::Shear:
        {
            float m11v = m11 * o.m11 + m12 * o.m21;
            float m12v = m11 * o.m12 + m12 * o.m22;

            float m21v = m21 * o.m11 + m22 * o.m21;
            float m22v = m21 * o.m12 + m22 * o.m22;

            float m31v = mtx * o.m11 + mty * o.m21 + o.mtx;
            float m32v = mtx * o.m12 + mty * o.m22 + o.mty;

            m11 = m11v;
            m12 = m12v;
            m21 = m21v;
            m22 = m22v;
            mtx = m31v;
            mty = m32v;
            break;
        }
        case MatrixType::Project:
        {
            float m11v = m11 * o.m11 + m12 * o.m21 + m13 * o.mtx;
            float m12v = m11 * o.m12 + m12 * o.m22 + m13 * o.mty;
            float m13v = m11 * o.m13 + m12 * o.m23 + m13 * o.m33;

            float m21v = m21 * o.m11 + m22 * o.m21 + m23 * o.mtx;
            float m22v = m21 * o.m12 + m22 * o.m22 + m23 * o.mty;
            float m23v = m21 * o.m13 + m22 * o.m23 + m23 * o.m33;

            float m31v = mtx * o.m11 + mty * o.m21 + m33 * o.mtx;
            float m32v = mtx * o.m12 + mty * o.m22 + m33 * o.mty;
            float m33v = mtx * o.m13 + mty * o.m23 + m33 * o.m33;

            m11 = m11v;
            m12 = m12v;
            m13 = m13v;
            m21 = m21v;
            m22 = m22v;
            m23 = m23v;
            mtx = m31v;
            mty = m32v;
            m33 = m33v;
        }
        }

        dirty = t;
        mType = t;

        return *this;
    }
    VMatrix &operator*=(float num) {
        if (num == 1.) return *this;

        m11 *= num;
        m12 *= num;
        m13 *= num;
        m21 *= num;
        m22 *= num;
        m23 *= num;
        mtx *= num;
        mty *= num;
        m33 *= num;
        if (dirty < MatrixType::Scale) dirty = MatrixType::Scale;

        return *this;
    }
    VMatrix &operator/=(float div) {
        if (div == 0) return *this;

        div = 1 / div;
        return operator*=(div);
    }
    bool                 operator==(const VMatrix &o) const { return fuzzyCompare(o); }
    bool                 operator!=(const VMatrix &o) const { return !operator==(o); }
    bool                 fuzzyCompare(const VMatrix &o) const {
        return vCompare(m11, o.m11) && vCompare(m12, o.m12) &&
               vCompare(m21, o.m21) && vCompare(m22, o.m22) &&
               vCompare(mtx, o.mtx) && vCompare(mty, o.mty);
    }
    float                scale() const {
        constexpr float SQRT_2 = 1.41421f;
        VPointF         p1(0, 0);
        VPointF         p2(SQRT_2, SQRT_2);
        p1 = map(p1);
        p2 = map(p2);
        VPointF final = p2 - p1;

        return std::sqrt(final.x() * final.x() + final.y() * final.y()) / 2.0f;
    }
private:
    friend struct VSpanData;
    float              m11{1}, m12{0}, m13{0};
    float              m21{0}, m22{1}, m23{0};
    float              mtx{0}, mty{0}, m33{1};
    mutable MatrixType mType{MatrixType::None};
    mutable MatrixType dirty{MatrixType::None};
};

template <typename T>
class vcow_ptr {
    struct model {
        std::atomic<std::size_t> mRef{1};
        model() = default;
        template <class... Args> explicit model(Args&&... args) : mValue(std::forward<Args>(args)...) {}
        explicit model(const T &other) : mValue(other) {}
        T mValue;
    };
    model *mModel;

public:
    using element_type = T;

    vcow_ptr() { static model default_s; mModel = &default_s; ++mModel->mRef; }
    ~vcow_ptr() { if (mModel && (--mModel->mRef == 0)) delete mModel; }

    template <class... Args>
    explicit vcow_ptr(Args&&... args) : mModel(new model(std::forward<Args>(args)...))     {    }

    vcow_ptr(const vcow_ptr &x) noexcept : mModel(x.mModel) { assert(mModel); ++mModel->mRef; }
    vcow_ptr(vcow_ptr &&x) noexcept : mModel(x.mModel) { assert(mModel); x.mModel = nullptr; }
    auto operator=(const vcow_ptr &x) noexcept -> vcow_ptr & { *this = vcow_ptr(x); return *this; }
    auto operator=(vcow_ptr &&x) noexcept -> vcow_ptr & { auto tmp = std::move(x); swap(*this, tmp); return *this; }
    auto operator*() const noexcept -> const element_type & { return read(); }
    auto operator-> () const noexcept -> const element_type * { return &read(); }
    std::size_t refCount() const noexcept { assert(mModel);  return mModel->mRef; }
    bool unique() const noexcept { assert(mModel); return mModel->mRef == 1; }
    auto write() -> element_type & { if (!unique()) *this = vcow_ptr(read()); return mModel->mValue; }
    auto read() const noexcept -> const element_type & { assert(mModel); return mModel->mValue; }
    friend inline void swap(vcow_ptr &x, vcow_ptr &y) noexcept { std::swap(x.mModel, y.mModel); }
};

class VRle {
public:
    struct Span {
        short  x{0};
        short  y{0};
        ushort len{0};
        uchar  coverage{0};
    };
    using VRleSpanCb = void (*)(size_t count, const VRle::Span *spans,
                                void *userData);
    bool  empty() const {
        return d->empty();
    }

    VRect boundingRect() const { return d->bbox(); }
    void setBoundingRect(const VRect &bbox) { d->setBbox(bbox); }
    void addSpan(const VRle::Span *span, size_t count) { d.write().addSpan(span, count); }

    void reset() { d.write().reset(); }
    void translate(const VPoint &p) { d.write().translate(p); }
    void invert() { d.write().invert(); }

    void operator*=(uchar alpha) { d.write() *= alpha; }

    void intersect(const VRect &r, VRleSpanCb cb, void *userData) const { d->opIntersect(r, cb, userData); }
    void intersect(const VRle &r, VRleSpanCb cb, void *userData) const {
        if (empty() || r.empty()) return;
        opIntersectHelper(d.read(), r.d.read(), cb, userData);
    }

    void operator&=(const VRle &o);
    VRle operator&(const VRle &o) const {
        if (empty() || o.empty()) return VRle();

        VRle result;
        result.d.write().opIntersect(d.read(), o.d.read());

        return result;
    }
    VRle operator-(const VRle &o) const {
        if (empty()) return VRle();
        if (o.empty()) return *this;

        VRle result;
        result.d.write().opSubstract(d.read(), o.d.read());

        return result;
    }
    VRle operator+(const VRle &o) const {
        if (empty()) return o;
        if (o.empty()) return *this;

        VRle result;
        result.d.write().opGeneric(d.read(), o.d.read(), VRleData::OpCode::Add);

        return result;
    }
    VRle operator^(const VRle &o) const {
        if (empty()) return o;
        if (o.empty()) return *this;

        VRle result;
        result.d.write().opGeneric(d.read(), o.d.read(), VRleData::OpCode::Xor);

        return result;
    }

    static VRle toRle(const VRect &rect);

    bool unique() const { return d.unique(); }
    size_t refCount() const { return d.refCount(); }
    void clone(const VRle &o) { d.write().clone(o.d.read()); }

public:
    struct VRleData {
        enum class OpCode {
            Add,
            Xor
        };
        bool  empty() const { return mSpans.empty(); }
        void  addSpan(const VRle::Span *span, size_t count);
        void  updateBbox() const;
        VRect bbox() const;
        void setBbox(const VRect &bbox) const;
        void  reset();
        void  translate(const VPoint &p);
        void  operator*=(uchar alpha);
        void  invert();
        void  opIntersect(const VRect &, VRle::VRleSpanCb, void *) const;
        void  opGeneric(const VRle::VRleData &, const VRle::VRleData &, OpCode code);
        void  opSubstract(const VRle::VRleData &, const VRle::VRleData &);
        void  opIntersect(const VRle::VRleData &, const VRle::VRleData &);
        void  addRect(const VRect &rect);
        void  clone(const VRle::VRleData &);
        std::vector<VRle::Span> mSpans;
        VPoint                  mOffset;
        mutable VRect           mBbox;
        mutable bool            mBboxDirty = true;
    };
private:
    friend void opIntersectHelper(const VRle::VRleData &obj1,
                                  const VRle::VRleData &obj2,
                                  VRle::VRleSpanCb cb, void *userData);
    vcow_ptr<VRleData> d;
};

struct VPathData;
class VPath {
public:
    enum class Direction { CCW, CW };

    enum class Element : uchar { MoveTo, LineTo, CubicTo, Close };
    bool  empty() const;
    bool  null() const;
    void  moveTo(const VPointF &p);
    void  moveTo(float x, float y);
    void  lineTo(const VPointF &p);
    void  lineTo(float x, float y);
    void  cubicTo(const VPointF &c1, const VPointF &c2, const VPointF &e);
    void  cubicTo(float c1x, float c1y, float c2x, float c2y, float ex,
                  float ey);
    void  arcTo(const VRectF &rect, float startAngle, float sweepLength,
                bool forceMoveTo);
    void  close();
    void  reset();
    void  reserve(size_t pts, size_t elms);
    size_t segments() const;
    void  addCircle(float cx, float cy, float radius,
                    VPath::Direction dir = Direction::CW);
    void  addOval(const VRectF &rect, VPath::Direction dir = Direction::CW);
    void  addRoundRect(const VRectF &rect, float rx, float ry,
                       VPath::Direction dir = Direction::CW);
    void  addRoundRect(const VRectF &rect, float roundness,
                       VPath::Direction dir = Direction::CW);
    void  addRect(const VRectF &rect, VPath::Direction dir = Direction::CW);
    void  addPolystar(float points, float innerRadius, float outerRadius,
                      float innerRoundness, float outerRoundness,
                      float startAngle, float cx, float cy,
                      VPath::Direction dir = Direction::CW);
    void  addPolygon(float points, float radius, float roundness,
                     float startAngle, float cx, float cy,
                     VPath::Direction dir = Direction::CW);
    void addPath(const VPath &path);
    void  addPath(const VPath &path, const VMatrix &m);
    void  transform(const VMatrix &m);
    float length() const;
    const std::vector<VPath::Element> &elements() const;
    const std::vector<VPointF> &       points() const;
    void  clone(const VPath &srcPath);
    bool unique() const { return d.unique();}
    size_t refCount() const { return d.refCount();}

private:
    struct VPathData {
        bool  empty() const { return m_elements.empty(); }
        bool  null() const { return empty() && !m_elements.capacity();}
        void  moveTo(float x, float y);
        void  lineTo(float x, float y);
        void  cubicTo(float cx1, float cy1, float cx2, float cy2, float ex, float ey);
        void  close();
        void  reset();
        void  reserve(size_t, size_t);
        void  checkNewSegment();
        size_t segments() const;
        void  transform(const VMatrix &m);
        float length() const;
        void  addRoundRect(const VRectF &, float, float, VPath::Direction);
        void  addRoundRect(const VRectF &, float, VPath::Direction);
        void  addRect(const VRectF &, VPath::Direction);
        void  arcTo(const VRectF &, float, float, bool);
        void  addCircle(float, float, float, VPath::Direction);
        void  addOval(const VRectF &, VPath::Direction);
        void  addPolystar(float points, float innerRadius, float outerRadius,
                          float innerRoundness, float outerRoundness,
                          float startAngle, float cx, float cy,
                          VPath::Direction dir = Direction::CW);
        void  addPolygon(float points, float radius, float roundness,
                         float startAngle, float cx, float cy,
                         VPath::Direction dir = Direction::CW);
        void  addPath(const VPathData &path, const VMatrix *m = nullptr);
        void  clone(const VPath::VPathData &o) { *this = o;}
        const std::vector<VPath::Element> &elements() const
        {
            return m_elements;
        }
        const std::vector<VPointF> &points() const { return m_points; }
        std::vector<VPointF>        m_points;
        std::vector<VPath::Element> m_elements;
        size_t                      m_segments;
        VPointF                     mStartPoint;
        mutable float               mLength{0};
        mutable bool                mLengthDirty{true};
        bool                        mNewSegment;
    };

    vcow_ptr<VPathData> d;
};

inline bool VPath::empty() const
{
    return d->empty();
}

/*
* path is empty as well as null(no memory for data allocated yet).
*/
inline bool VPath::null() const
{
    return d->null();
}

inline void VPath::moveTo(const VPointF &p)
{
    d.write().moveTo(p.x(), p.y());
}

inline void VPath::lineTo(const VPointF &p)
{
    d.write().lineTo(p.x(), p.y());
}

inline void VPath::close()
{
    d.write().close();
}

inline void VPath::reset()
{
    d.write().reset();
}

inline void VPath::reserve(size_t pts, size_t elms)
{
    d.write().reserve(pts, elms);
}

inline size_t VPath::segments() const
{
    return d->segments();
}

inline float VPath::length() const
{
    return d->length();
}

inline void VPath::cubicTo(const VPointF &c1, const VPointF &c2,
                           const VPointF &e)
{
    d.write().cubicTo(c1.x(), c1.y(), c2.x(), c2.y(), e.x(), e.y());
}

inline void VPath::lineTo(float x, float y)
{
    d.write().lineTo(x, y);
}

inline void VPath::moveTo(float x, float y)
{
    d.write().moveTo(x, y);
}

inline void VPath::cubicTo(float c1x, float c1y, float c2x, float c2y, float ex,
                           float ey)
{
    d.write().cubicTo(c1x, c1y, c2x, c2y, ex, ey);
}

inline void VPath::transform(const VMatrix &m)
{
    d.write().transform(m);
}

inline void VPath::arcTo(const VRectF &rect, float startAngle,
                         float sweepLength, bool forceMoveTo)
{
    d.write().arcTo(rect, startAngle, sweepLength, forceMoveTo);
}

inline void VPath::addRect(const VRectF &rect, VPath::Direction dir)
{
    d.write().addRect(rect, dir);
}

inline void VPath::addRoundRect(const VRectF &rect, float rx, float ry,
                                VPath::Direction dir)
{
    d.write().addRoundRect(rect, rx, ry, dir);
}

inline void VPath::addRoundRect(const VRectF &rect, float roundness,
                                VPath::Direction dir)
{
    d.write().addRoundRect(rect, roundness, dir);
}

inline void VPath::addCircle(float cx, float cy, float radius,
                             VPath::Direction dir)
{
    d.write().addCircle(cx, cy, radius, dir);
}

inline void VPath::addOval(const VRectF &rect, VPath::Direction dir)
{
    d.write().addOval(rect, dir);
}

inline void VPath::addPolystar(float points, float innerRadius,
                               float outerRadius, float innerRoundness,
                               float outerRoundness, float startAngle, float cx,
                               float cy, VPath::Direction dir)
{
    d.write().addPolystar(points, innerRadius, outerRadius, innerRoundness,
                          outerRoundness, startAngle, cx, cy, dir);
}

inline void VPath::addPolygon(float points, float radius, float roundness,
                              float startAngle, float cx, float cy,
                              VPath::Direction dir)
{
    d.write().addPolygon(points, radius, roundness, startAngle, cx, cy, dir);
}

inline void VPath::addPath(const VPath &path)
{
    if (path.empty()) return;

    if (null()) {
        *this = path;
    } else {
        d.write().addPath(path.d.read());
    }
}

inline void  VPath::addPath(const VPath &path, const VMatrix &m)
{
    if (path.empty()) return;

    d.write().addPath(path.d.read(), &m);
}

inline const std::vector<VPath::Element> &VPath::elements() const
{
    return d->elements();
}

inline const std::vector<VPointF> &VPath::points() const
{
    return d->points();
}

inline void VPath::clone(const VPath &o)
{
    d.write().clone(o.d.read());
}

class VBezier {
public:
    VBezier() = default;
    VPointF     pointAt(float t) const;
    float       angleAt(float t) const;
    VBezier     onInterval(float t0, float t1) const;
    float       length() const;
    static void coefficients(float t, float &a, float &b, float &c, float &d);
    static VBezier fromPoints(const VPointF &start, const VPointF &cp1,
                              const VPointF &cp2, const VPointF &end);
    inline void    parameterSplitLeft(float t, VBezier *left);
    inline void    split(VBezier *firstHalf, VBezier *secondHalf) const;
    float          tAtLength(float len) const;
    void           splitAtLength(float len, VBezier *left, VBezier *right);
    VPointF        pt1() const { return {x1, y1}; }
    VPointF        pt2() const { return {x2, y2}; }
    VPointF        pt3() const { return {x3, y3}; }
    VPointF        pt4() const { return {x4, y4}; }

private:
    VPointF derivative(float t) const;
    float   x1, y1, x2, y2, x3, y3, x4, y4;
};

class VDasher {
public:
    VDasher(const float *dashArray, size_t size) {
        mDashArray = reinterpret_cast<const VDasher::Dash *>(dashArray);
        mArraySize = size / 2;
        if (size % 2) mDashOffset = dashArray[size - 1];
        mIndex = 0;
        mCurrentLength = 0;
        mDiscard = false;
        //if the dash array contains ZERO length
        // segments or ZERO lengths gaps we could
        // optimize those usecase.
        for (size_t i = 0; i < mArraySize; i++) {
            if (!vCompare(mDashArray[i].length, 0.0f))
                mNoLength = false;
            if (!vCompare(mDashArray[i].gap, 0.0f))
                mNoGap = false;
        }
    }
    VPath dashed(const VPath &path) {
        if (mNoLength && mNoGap) return path;

        if (path.empty() || mNoLength) return VPath();

        if (mNoGap) return path;

        VPath result;

        dashHelper(path, result);

        return result;
    }
    void dashed(const VPath &path, VPath &result) {
        if (mNoLength && mNoGap) return result.reset();

        if (path.empty() || mNoLength) return result.reset();

        if (mNoGap) return result.clone(path);

        result.reset();

        dashHelper(path, result);
    }

private:
    static constexpr float tolerance = 0.1f;
    void moveTo(const VPointF &p) {
        mDiscard = false;
        mStartNewSegment = true;
        mCurPt = p;
        mIndex = 0;

        if (!vCompare(mDashOffset, 0.0f)) {
            float totalLength = 0.0;
            for (size_t i = 0; i < mArraySize; i++) {
                totalLength = mDashArray[i].length + mDashArray[i].gap;
            }
            float normalizeLen = std::fmod(mDashOffset, totalLength);
            if (normalizeLen < 0.0f) {
                normalizeLen = totalLength + normalizeLen;
            }
            // now the length is less than total length and +ve
            // findout the current dash index , dashlength and gap.
            for (size_t i = 0; i < mArraySize; i++) {
                if (normalizeLen < mDashArray[i].length) {
                    mIndex = i;
                    mCurrentLength = mDashArray[i].length - normalizeLen;
                    mDiscard = false;
                    break;
                }
                normalizeLen -= mDashArray[i].length;
                if (normalizeLen < mDashArray[i].gap) {
                    mIndex = i;
                    mCurrentLength = mDashArray[i].gap - normalizeLen;
                    mDiscard = true;
                    break;
                }
                normalizeLen -= mDashArray[i].gap;
            }
        } else {
            mCurrentLength = mDashArray[mIndex].length;
        }
        if (vIsZero(mCurrentLength)) updateActiveSegment();
    }
    void lineTo(const VPointF &p) {
        VLine left, right;
        VLine line(mCurPt, p);
        float length = line.length();

        if (length <= mCurrentLength) {
            mCurrentLength -= length;
            addLine(p);
        } else {
            while (length > mCurrentLength) {
                length -= mCurrentLength;
                line.splitAtLength(mCurrentLength, left, right);

                addLine(left.p2());
                updateActiveSegment();

                line = right;
                mCurPt = line.p1();
            }
            // handle remainder
            if (length > tolerance) {
                mCurrentLength -= length;
                addLine(line.p2());
            }
        }

        if (mCurrentLength < tolerance) updateActiveSegment();

        mCurPt = p;
    }
    void cubicTo(const VPointF &cp1, const VPointF &cp2, const VPointF &e) {
        VBezier left, right;
        VBezier b = VBezier::fromPoints(mCurPt, cp1, cp2, e);
        float   bezLen = b.length();

        if (bezLen <= mCurrentLength) {
            mCurrentLength -= bezLen;
            addCubic(cp1, cp2, e);
        } else {
            while (bezLen > mCurrentLength) {
                bezLen -= mCurrentLength;
                b.splitAtLength(mCurrentLength, &left, &right);

                addCubic(left.pt2(), left.pt3(), left.pt4());
                updateActiveSegment();

                b = right;
                mCurPt = b.pt1();
            }
            // handle remainder
            if (bezLen > tolerance) {
                mCurrentLength -= bezLen;
                addCubic(b.pt2(), b.pt3(), b.pt4());
            }
        }

        if (mCurrentLength < tolerance) updateActiveSegment();

        mCurPt = e;
    }
    void close();
    void addLine(const VPointF &p) {
        if (mDiscard) return;

        if (mStartNewSegment) {
            mResult->moveTo(mCurPt);
            mStartNewSegment = false;
        }
        mResult->lineTo(p);
    }

    void addCubic(const VPointF &cp1, const VPointF &cp2, const VPointF &e) {
        if (mDiscard) return;

        if (mStartNewSegment) {
            mResult->moveTo(mCurPt);
            mStartNewSegment = false;
        }
        mResult->cubicTo(cp1, cp2, e);
    }
    void updateActiveSegment() {
        mStartNewSegment = true;

        if (mDiscard) {
            mDiscard = false;
            mIndex = (mIndex + 1) % mArraySize;
            mCurrentLength = mDashArray[mIndex].length;
        } else {
            mDiscard = true;
            mCurrentLength = mDashArray[mIndex].gap;
        }
        if (vIsZero(mCurrentLength)) updateActiveSegment();
    }


private:
    void dashHelper(const VPath &path, VPath &result) {
        mResult = &result;
        mResult->reserve(path.points().size(), path.elements().size());
        mIndex = 0;
        const std::vector<VPath::Element> &elms = path.elements();
        const std::vector<VPointF> &       pts = path.points();
        const VPointF *                    ptPtr = pts.data();

        for (auto &i : elms) {
            switch (i) {
            case VPath::Element::MoveTo: {
                moveTo(*ptPtr++);
                break;
            }
            case VPath::Element::LineTo: {
                lineTo(*ptPtr++);
                break;
            }
            case VPath::Element::CubicTo: {
                cubicTo(*ptPtr, *(ptPtr + 1), *(ptPtr + 2));
                ptPtr += 3;
                break;
            }
            case VPath::Element::Close: {
                // The point is already joined to start point in VPath
                // no need to do anything here.
                break;
            }
            }
        }
        mResult = nullptr;
    }
    struct Dash {
        float length;
        float gap;
    };
    const VDasher::Dash *mDashArray;
    size_t               mArraySize{0};
    VPointF              mCurPt;
    size_t               mIndex{0}; /* index to the dash Array */
    float                mCurrentLength;
    float                mDashOffset{0};
    VPath               *mResult{nullptr};
    bool                 mDiscard{false};
    bool                 mStartNewSegment{true};
    bool                 mNoLength{true};
    bool                 mNoGap{true};
};

class VPathMesure {
public:
    void setRange(float start, float end) {mStart = start; mEnd = end;}
    void  setStart(float start) {mStart = start;}
    void  setEnd(float end) {mEnd = end;}
    VPath trim(const VPath &path) {
        if (vCompare(mStart, mEnd)) return VPath();

        if ((vCompare(mStart, 0.0f) && (vCompare(mEnd, 1.0f))) ||
                (vCompare(mStart, 1.0f) && (vCompare(mEnd, 0.0f))))
            return path;

        float length = path.length();

        if (mStart < mEnd) {
            float array[4] = {
                0.0f, length * mStart,  // 1st segment
                (mEnd - mStart) * length,
                std::numeric_limits<float>::max(),  // 2nd segment
            };
            VDasher dasher(array, 4);
            dasher.dashed(path, mScratchObject);
            return mScratchObject;
        } else {
            float array[4] = {
                length * mEnd, (mStart - mEnd) * length,  // 1st segment
                (1 - mStart) * length,
                std::numeric_limits<float>::max(),  // 2nd segment
            };
            VDasher dasher(array, 4);
            dasher.dashed(path, mScratchObject);
            return mScratchObject;
        }
    }
private:
    float mStart{0.0f};
    float mEnd{1.0f};
    VPath mScratchObject;
};

class VRasterizer
{
public:
    void rasterize(VPath path, FillRule fillRule = FillRule::Winding, const VRect &clip = VRect());
    void rasterize(VPath path, CapStyle cap, JoinStyle join, float width,
                   float miterLimit, const VRect &clip = VRect());
    VRle rle();
private:
    struct VRasterizerImpl;
    void init();
    void updateRequest();
    std::shared_ptr<VRasterizerImpl> d{nullptr};
};

class VBitmap {
public:
    enum class Format : uchar {
        Invalid,
        Alpha8,
        ARGB32,
        ARGB32_Premultiplied
    };

    VBitmap() = default;
    VBitmap(size_t w, size_t h, VBitmap::Format format);
    VBitmap(uchar *data, size_t w, size_t h, size_t bytesPerLine, VBitmap::Format format);
    void reset(uchar *data, size_t w, size_t h, size_t stride, VBitmap::Format format);
    void reset(size_t w, size_t h, VBitmap::Format format = Format::ARGB32_Premultiplied);
    size_t          stride() const;
    size_t          width() const;
    size_t          height() const;
    size_t          depth() const;
    VBitmap::Format format() const;
    bool            valid() const;
    uchar *         data();
    uchar *         data() const;
    VRect           rect() const;
    VSize           size() const;
    bool    isNeedClear() const { return mImpl ? mImpl->mNeedClear : true; }
    void    setNeedClear(bool needClear) { if (mImpl) mImpl->mNeedClear = needClear; }
    void    fill(uint pixel);
    void    updateLuma();
private:
    struct Impl {
        std::unique_ptr<uchar[]> mOwnData{nullptr};
        uchar *         mRoData{nullptr};
        uint            mWidth{0};
        uint            mHeight{0};
        uint            mStride{0};
        uchar           mDepth{0};
        bool            mNeedClear{true};
        VBitmap::Format mFormat{VBitmap::Format::Invalid};

        explicit Impl(size_t width, size_t height, VBitmap::Format format)
        {
            reset(width, height, format);
        }
        explicit Impl(uchar *data, size_t w, size_t h, size_t bytesPerLine, VBitmap::Format format)
        {
            reset(data, w, h, bytesPerLine, format);
        }
        VRect   rect() const { return VRect(0, 0, mWidth, mHeight);}
        VSize   size() const { return VSize(mWidth, mHeight); }
        size_t  stride() const { return mStride; }
        size_t  width() const { return mWidth; }
        size_t  height() const { return mHeight; }
        uchar * data() { return mRoData ? mRoData : mOwnData.get(); }
        VBitmap::Format format() const { return mFormat; }
        void reset(uchar *, size_t, size_t, size_t, VBitmap::Format);
        void reset(size_t, size_t, VBitmap::Format);
        static uchar depth(VBitmap::Format format);
        void fill(uint);
        void updateLuma();
    };

    std::shared_ptr<Impl> mImpl;
};

using VGradientStop = std::pair<float, VColor>;
using VGradientStops = std::vector<VGradientStop>;
class VGradient {
public:
    enum class Mode { Absolute, Relative };
    enum class Spread { Pad, Repeat, Reflect };
    enum class Type { Linear, Radial };
    explicit VGradient(VGradient::Type type);
    void setStops(const VGradientStops &stops);
    void setAlpha(float alpha) {mAlpha = alpha;}
    float alpha() const {return mAlpha;}

public:
    static constexpr int colorTableSize = 1024;
    VGradient::Type      mType{Type::Linear};
    VGradient::Spread    mSpread{Spread::Pad};
    VGradient::Mode      mMode{Mode::Absolute};
    VGradientStops       mStops;
    float                mAlpha{1.0};
    struct Linear {
        float x1{0}, y1{0}, x2{0}, y2{0};
    };
    struct Radial {
        float cx{0}, cy{0}, fx{0}, fy{0}, cradius{0}, fradius{0};
    };
    union {
        Linear linear;
        Radial radial;
    };
    VMatrix mMatrix;
};

struct VTexture {
    VBitmap  mBitmap;
    VMatrix  mMatrix;
    int      mAlpha{255};
};

class VBrush {
public:
    enum class Type { NoBrush, Solid, LinearGradient, RadialGradient, Texture };
    VBrush(): mType(Type::NoBrush), mColor() {};
    explicit VBrush(const VColor &color);
    explicit VBrush(const VGradient *gradient);
    explicit VBrush(uchar r, uchar g, uchar b, uchar a);
    explicit VBrush(const VTexture *texture);
    inline VBrush::Type type() const { return mType; }
public:
    VBrush::Type     mType{Type::NoBrush};
    union {
        VColor           mColor{};
        const VGradient *mGradient;
        const VTexture  *mTexture;
    };
};

struct Operator;
struct VSpanData;
typedef void (*CompositionFunctionSolid)(uint32_t *dest, int length, uint32_t color, uint32_t const_alpha);
typedef void (*CompositionFunction)(uint32_t *dest, const uint32_t *src, int length, uint32_t const_alpha);
typedef void (*SourceFetchProc)(uint32_t *buffer, const Operator *o, const VSpanData *data, int y, int x, int length);
typedef void (*ProcessRleSpan)(size_t count, const VRle::Span *spans, void *userData);

struct LinearGradientValues {
    float dx;
    float dy;
    float l;
    float off;
};

struct RadialGradientValues {
    float dx;
    float dy;
    float dr;
    float sqrfr;
    float a;
    float inv2a;
    bool  extended;
};

struct Operator {
    BlendMode                 mode;
    SourceFetchProc           srcFetch;
    CompositionFunctionSolid  funcSolid;
    CompositionFunction       func;
    union {
        LinearGradientValues linear;
        RadialGradientValues radial;
    };
};

class VRasterBuffer {
public:
    VBitmap::Format prepare(VBitmap *image);
    void            clear();

    void resetBuffer(int val = 0);

    inline uchar *scanLine(int y)
    {
        assert(y >= 0);
        assert(size_t(y) < mHeight);
        return mBuffer + y * mBytesPerLine;
    }

    size_t width() const { return mWidth; }
    size_t height() const { return mHeight; }
    size_t bytesPerLine() const { return mBytesPerLine; }
    size_t bytesPerPixel() const { return mBytesPerPixel; }

    VBitmap::Format           mFormat{VBitmap::Format::ARGB32_Premultiplied};
private:
    size_t    mWidth{0};
    size_t    mHeight{0};
    size_t    mBytesPerLine{0};
    size_t    mBytesPerPixel{0};
    uchar    *mBuffer{nullptr};
    bool      mNeedClear{true};
};

struct VGradientData {
    VGradient::Spread mSpread;
    struct Linear {
        float x1, y1, x2, y2;
    };
    struct Radial {
        float cx, cy, fx, fy, cradius, fradius;
    };
    union {
        Linear linear;
        Radial radial;
    };
    const uint32_t *mColorTable;
    bool            mColorTableAlpha;
};

struct VBitmapData
{
    const uchar *imageData;
    const uchar *scanLine(int y) const { return imageData + y * bytesPerLine; }

    int width;
    int height;
    // clip rect
    int x1;
    int y1;
    int x2;
    int y2;
    uint bytesPerLine;
    VBitmap::Format format;
    bool hasAlpha;
    enum Type {
        Plain,
        Tiled
    };
    Type type;
    int const_alpha;
};

struct VColorTable
{
    uint32_t buffer32[VGradient::colorTableSize];
    bool     alpha{true};
};

struct VSpanData {
    enum class Type { None, Solid, LinearGradient, RadialGradient, Texture };

    void  updateSpanFunc();
    void  init(VRasterBuffer *image);
    void  setup(const VBrush &            brush,
                BlendMode mode = BlendMode::SrcOver,
                int                       alpha = 255);
    void  setupMatrix(const VMatrix &matrix);

    VRect clipRect() const
    {
        return VRect(0, 0, mDrawableSize.width(), mDrawableSize.height());
    }

    void setDrawRegion(const VRect &region)
    {
        mOffset = VPoint(region.left(), region.top());
        mDrawableSize = VSize(region.width(), region.height());
    }

    uint *buffer(int x, int y) const
    {
        return (uint *)(mRasterBuffer->scanLine(y + mOffset.y())) + x + mOffset.x();
    }
    void initTexture(const VBitmap *image, int alpha, VBitmapData::Type type, const VRect &sourceRect);

    BlendMode                            mBlendMode{BlendMode::SrcOver};
    VRasterBuffer *                      mRasterBuffer;
    ProcessRleSpan                       mBlendFunc;
    ProcessRleSpan                       mUnclippedBlendFunc;
    VSpanData::Type                      mType;
    std::shared_ptr<const VColorTable>   mColorTable{nullptr};
    VPoint                               mOffset; // offset to the subsurface
    VSize                                mDrawableSize;// suburface size
    union {
        uint32_t      mSolid;
        VGradientData mGradient;
        VBitmapData   mBitmap;
    };
    float m11, m12, m13, m21, m22, m23, m33, dx, dy;  // inverse xform matrix
    bool fast_matrix{true};
    VMatrix::MatrixType   transformType{VMatrix::MatrixType::None};
};

void        vInitDrawhelperFunctions();
extern void vInitBlendFunctions();

#define BYTE_MUL(c, a)                                  \
    ((((((c) >> 8) & 0x00ff00ff) * (a)) & 0xff00ff00) + \
     (((((c)&0x00ff00ff) * (a)) >> 8) & 0x00ff00ff))

inline constexpr int vRed(uint32_t c)
{
    return ((c >> 16) & 0xff);
}

inline constexpr int vGreen(uint32_t c)
{
    return ((c >> 8) & 0xff);
}

inline constexpr int vBlue(uint32_t c)
{
    return (c & 0xff);
}

inline constexpr int vAlpha(uint32_t c)
{
    return c >> 24;
}

static inline uint INTERPOLATE_PIXEL_255(uint x, uint a, uint y, uint b)
{
    uint t = (x & 0xff00ff) * a + (y & 0xff00ff) * b;
    t >>= 8;
    t &= 0xff00ff;
    x = ((x >> 8) & 0xff00ff) * a + ((y >> 8) & 0xff00ff) * b;
    x &= 0xff00ff00;
    x |= t;
    return x;
}

#define LOOP_ALIGNED_U1_A4(DEST, LENGTH, UOP, A4OP) \
    {                                               \
        while ((uintptr_t)DEST & 0xF && LENGTH)     \
            UOP                                     \
                                                    \
                while (LENGTH)                      \
            {                                       \
                switch (LENGTH) {                   \
                case 3:                             \
                case 2:                             \
                case 1:                             \
                    UOP break;                      \
                default:                            \
                    A4OP break;                     \
                }                                   \
            }                                       \
    }

class VPainter {
public:
    VPainter() = default;
    explicit VPainter(VBitmap *buffer);
    bool  begin(VBitmap *buffer);
    void  end();
    void  setDrawRegion(const VRect &region); // sub surface rendering area.
    void  setBrush(const VBrush &brush);
    void  setBlendMode(BlendMode mode);
    void  drawRle(const VPoint &pos, const VRle &rle);
    void  drawRle(const VRle &rle, const VRle &clip);
    VRect clipBoundingRect() const;

    void  drawBitmap(const VPoint &point, const VBitmap &bitmap, const VRect &source, uint8_t const_alpha = 255);
    void  drawBitmap(const VRect &target, const VBitmap &bitmap, const VRect &source, uint8_t const_alpha = 255);
    void  drawBitmap(const VPoint &point, const VBitmap &bitmap, uint8_t const_alpha = 255);
    void  drawBitmap(const VRect &rect, const VBitmap &bitmap, uint8_t const_alpha = 255);
private:
    void drawBitmapUntransform(const VRect &target, const VBitmap &bitmap,
                               const VRect &source, uint8_t const_alpha);
    VRasterBuffer mBuffer;
    VSpanData     mSpanData;
};

class VDrawable {
public:
    enum class DirtyState : unsigned char {
        None = 1 << 0,
        Path = 1 << 1,
        Stroke = 1 << 2,
        Brush = 1 << 3,
        All = (Path | Stroke | Brush)
    };

    enum class Type : unsigned char {
        Fill,
        Stroke,
        StrokeWithDash
    };

    explicit VDrawable(VDrawable::Type type = Type::Fill);
    void setType(VDrawable::Type type);
    ~VDrawable();

    typedef vFlag<DirtyState> DirtyFlag;
    void setPath(const VPath &path);
    void setFillRule(FillRule rule) { mFillRule = rule; }
    void setBrush(const VBrush &brush) { mBrush = brush; }
    void setStrokeInfo(CapStyle cap, JoinStyle join, float miterLimit,
                       float strokeWidth);
    void setDashInfo(std::vector<float> &dashInfo);
    void preprocess(const VRect &clip);
    void applyDashOp();
    VRle rle();
    void setName(const char *name)
    {
        mName = name;
    }
    const char* name() const { return mName; }

public:
    struct StrokeInfo {
        float              width{0.0};
        float              miterLimit{10};
        CapStyle           cap{CapStyle::Flat};
        JoinStyle          join{JoinStyle::Bevel};
    };

    struct StrokeWithDashInfo : public StrokeInfo {
        std::vector<float> mDash;
    };

public:
    VPath                    mPath;
    VBrush                   mBrush;
    VRasterizer              mRasterizer;
    StrokeInfo              *mStrokeInfo{nullptr};

    DirtyFlag                mFlag{DirtyState::All};
    FillRule                 mFillRule{FillRule::Winding};
    VDrawable::Type          mType{Type::Fill};

    const char              *mName{nullptr};
};

class VImageLoader
{
public:
    static VImageLoader &instance();

    VBitmap load(const char *fileName);
    VBitmap load(const char *data, size_t len);
    ~VImageLoader();

private:
    VImageLoader();
    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

class VInterpolator {
public:
    static constexpr int kSplineTableSize = 11;
    VInterpolator() { /* caller must call Init later */ }

    VInterpolator(float aX1, float aY1, float aX2, float aY2) { init(aX1, aY1, aX2, aY2); }
    VInterpolator(VPointF pt1, VPointF pt2) { init(pt1.x(), pt1.y(), pt2.x(), pt2.y()); }

    void init(float aX1, float aY1, float aX2, float aY2);
    float value(float aX) const;
    void GetSplineDerivativeValues(float aX, float& aDX, float& aDY) const;

private:
    void CalcSampleValues();

    /**
    * Returns x(t) given t, x1, and x2, or y(t) given t, y1, and y2.
    */
    static float CalcBezier(float aT, float aA1, float aA2);

    /**
    * Returns dx/dt given t, x1, and x2, or dy/dt given t, y1, and y2.
    */
    static float GetSlope(float aT, float aA1, float aA2);

    float GetTForX(float aX) const;

    float NewtonRaphsonIterate(float aX, float aGuessT) const;

    float BinarySubdivide(float aX, float aA, float aB) const;

    static float A(float aA1, float aA2) { return 1.0f - 3.0f * aA2 + 3.0f * aA1; }
    static float B(float aA1, float aA2) { return 3.0f * aA2 - 6.0f * aA1; }
    static float C(float aA1) { return 3.0f * aA1; }

    float mX1;
    float mY1;
    float mX2;
    float mY2;
    float              mSampleValues[kSplineTableSize];
};

class VArenaAlloc {
public:
    VArenaAlloc(char*, size_t, size_t) {}

    explicit VArenaAlloc(size_t firstHeapAllocation)
        : VArenaAlloc(nullptr, 0, firstHeapAllocation)
    {}

    ~VArenaAlloc() {}

    template <typename T, typename... Args>
    T* make(Args&&... args) {
        return new T(std::forward<Args>(args)...);
    }

    template <typename T>
    T* makeArrayDefault(size_t count) {
        return new T[count];
    }

    template <typename T>
    T* makeArray(size_t count) {
        return new T[count];
    }
};

using lottie_image_load_f = unsigned char *(*)(const char *filename, int *x, int *y, int *comp, int req_comp);
using lottie_image_load_data_f = unsigned char *(*)(unsigned char const *data, int len, int *x, int *y, int *comp, int req_comp);
using lottie_image_free_f = void (*)(void *);

struct VImageLoader::Impl {
    lottie_image_load_f      imageLoad{nullptr};
    lottie_image_free_f      imageFree{nullptr};
    lottie_image_load_data_f imageFromData{nullptr};

    void  init() {
        imageLoad = stbi_load;
        imageFree = stbi_image_free;
        imageFromData = stbi_load_from_memory;
    }
    void moduleFree() {}

    Impl() { init(); }

    ~Impl() { moduleFree(); }

    VBitmap createBitmap(unsigned char *data, int width, int height,
                         int channel)
    {
        // premultiply alpha
        if (channel == 4)
            convertToBGRAPremul(data, width, height);
        else
            convertToBGRA(data, width, height);

        // create a bitmap of same size.
        VBitmap result =
            VBitmap(width, height, VBitmap::Format::ARGB32_Premultiplied);

        // copy the data to bitmap buffer
        memcpy(result.data(), data, width * height * 4);

        // free the image data
        imageFree(data);

        return result;
    }

    VBitmap load(const char *fileName)
    {
        if (!imageLoad) return VBitmap();

        int            width, height, n;
        unsigned char *data = imageLoad(fileName, &width, &height, &n, 4);

        if (!data) {
            return VBitmap();
        }

        return createBitmap(data, width, height, n);
    }

    VBitmap load(const char *imageData, size_t len)
    {
        if (!imageFromData) return VBitmap();

        int            width, height, n;
        unsigned char *data =
            imageFromData((unsigned char*)imageData, static_cast<int>(len), &width, &height, &n, 4);

        if (!data) {
            return VBitmap();
        }

        return createBitmap(data, width, height, n);
    }
    /*
    * convert from RGBA to BGRA and premultiply
    */
    void convertToBGRAPremul(unsigned char *bits, int width, int height)
    {
        int            pixelCount = width * height;
        unsigned char *pix = bits;
        for (int i = 0; i < pixelCount; i++) {
            unsigned char r = pix[0];
            unsigned char g = pix[1];
            unsigned char b = pix[2];
            unsigned char a = pix[3];

            r = (r * a) / 255;
            g = (g * a) / 255;
            b = (b * a) / 255;

            pix[0] = b;
            pix[1] = g;
            pix[2] = r;

            pix += 4;
        }
    }

    void convertToBGRA(unsigned char *bits, int width, int height)
    {
        int            pixelCount = width * height;
        unsigned char *pix = bits;
        for (int i = 0; i < pixelCount; i++) {
            unsigned char r = pix[0];
            unsigned char b = pix[2];
            pix[0] = b;
            pix[2] = r;
            pix += 4;
        }
    }
};

enum class MatteType: uchar
{
    None = 0,
    Alpha = 1,
    AlphaInv,
    Luma,
    LumaInv
};

enum class LayerType: uchar
{
    Precomp = 0,
    Solid = 1,
    Image = 2,
    Null = 3,
    Shape = 4,
    Text = 5
};

class LOTCompositionData;
class LOTLayerData;
class LOTTransformData;
class LOTShapeGroupData;
class LOTShapeData;
class LOTRectData;
class LOTEllipseData;
class LOTTrimData;
class LOTRepeaterData;
class LOTFillData;
class LOTStrokeData;
class LOTGroupData;
class LOTGFillData;
class LOTGStrokeData;
class LottieShapeData;
class LOTPolystarData;
class LOTMaskData;

struct LOTModelStat
{
    uint16_t precompLayerCount{0};
    uint16_t solidLayerCount{0};
    uint16_t shapeLayerCount{0};
    uint16_t imageLayerCount{0};
    uint16_t nullLayerCount{0};

};

template<typename T>
inline T lerp(const T& start, const T& end, float t)
{
    return start + t * (end - start);
}

template <typename T>
struct LOTKeyFrameValue {
    T mStartValue;
    T mEndValue;
    T value(float t) const { return lerp(mStartValue, mEndValue, t); }
    float angle(float ) const { return 0;}
};

class LottieColor
{
public:
    LottieColor() = default;
    LottieColor(float red, float green , float blue):r(red), g(green),b(blue){}
    VColor toColor(float a=1){ return VColor(uchar(255 * r),
                                             uchar(255 * g),
                                             uchar(255 * b),
                                             uchar(255 * a));}
    friend inline LottieColor operator+(const LottieColor &c1, const LottieColor &c2);
    friend inline LottieColor operator-(const LottieColor &c1, const LottieColor &c2);
public:
    float r{1};
    float g{1};
    float b{1};
};

class LOTKeyPath{
public:
    LOTKeyPath(const std::string &keyPath);
    bool matches(const std::string &key, uint depth);
    uint nextDepth(const std::string key, uint depth);
    bool fullyResolvesTo(const std::string key, uint depth);

    bool propagate(const std::string key, uint depth) {
        return skip(key) ? true : (depth < size()) || (mKeys[depth] == "**");
    }
    bool skip(const std::string &key) const { return key == "__";}
private:
    bool isGlobstar(uint depth) const {return mKeys[depth] == "**";}
    bool isGlob(uint depth) const {return mKeys[depth] == "*";}
    bool endsWithGlobstar() const { return mKeys.back() == "**"; }
    size_t size() const {return mKeys.size() - 1;}
private:
    std::vector<std::string> mKeys;
};

enum class Property {
    FillColor,     /*!< Color property of Fill object , value type is imlottie::Color */
    FillOpacity,   /*!< Opacity property of Fill object , value type is float [ 0 .. 100] */
    StrokeColor,   /*!< Color property of Stroke object , value type is imlottie::Color */
    StrokeOpacity, /*!< Opacity property of Stroke object , value type is float [ 0 .. 100] */
    StrokeWidth,   /*!< stroke with property of Stroke object , value type is float */
    TrAnchor,      /*!< Transform Anchor property of Layer and Group object , value type is imlottie::Point */
    TrPosition,    /*!< Transform Position property of Layer and Group object , value type is imlottie::Point */
    TrScale,       /*!< Transform Scale property of Layer and Group object , value type is imlottie::Size. range[0 ..100] */
    TrRotation,    /*!< Transform Scale property of Layer and Group object , value type is float. range[0 .. 360] in degrees*/
    TrOpacity      /*!< Transform Opacity property of Layer and Group object , value type is float [ 0 .. 100] */
};

struct FrameInfo {
    explicit FrameInfo(uint32_t frame): _frameNo(frame){}
    uint32_t curFrame() const {return _frameNo;}
private:
    uint32_t _frameNo;
};

// Naive way to implement std::variant
// refactor it when we move to c++17
// users should make sure proper combination
// of id and value are passed while creating the object.
class LOTVariant
{
public:
    using ValueFunc = std::function<float(const FrameInfo &)>;
    using ColorFunc = std::function<Color(const imlottie::FrameInfo &)>;
    using PointFunc = std::function<Point(const imlottie::FrameInfo &)>;
    using SizeFunc = std::function<Size(const imlottie::FrameInfo &)>;

    LOTVariant(imlottie::Property prop, const ValueFunc &v):mPropery(prop), mTag(Value)
    {
        construct(impl.valueFunc, v);
    }

    LOTVariant(imlottie::Property prop, ValueFunc &&v):mPropery(prop), mTag(Value)
    {
        moveConstruct(impl.valueFunc, std::move(v));
    }

    LOTVariant(imlottie::Property prop, const ColorFunc &v):mPropery(prop), mTag(Color)
    {
        construct(impl.colorFunc, v);
    }

    LOTVariant(imlottie::Property prop, ColorFunc &&v):mPropery(prop), mTag(Color)
    {
        moveConstruct(impl.colorFunc, std::move(v));
    }

    LOTVariant(imlottie::Property prop, const PointFunc &v):mPropery(prop), mTag(Point)
    {
        construct(impl.pointFunc, v);
    }

    LOTVariant(imlottie::Property prop, PointFunc &&v):mPropery(prop), mTag(Point)
    {
        moveConstruct(impl.pointFunc, std::move(v));
    }

    LOTVariant(imlottie::Property prop, const SizeFunc &v):mPropery(prop), mTag(Size)
    {
        construct(impl.sizeFunc, v);
    }

    LOTVariant(imlottie::Property prop, SizeFunc &&v):mPropery(prop), mTag(Size)
    {
        moveConstruct(impl.sizeFunc, std::move(v));
    }

    imlottie::Property property() const { return mPropery; }

    const ColorFunc& color() const
    {
        assert(mTag == Color);
        return impl.colorFunc;
    }

    const ValueFunc& value() const
    {
        assert(mTag == Value);
        return impl.valueFunc;
    }

    const PointFunc& point() const
    {
        assert(mTag == Point);
        return impl.pointFunc;
    }

    const SizeFunc& size() const
    {
        assert(mTag == Size);
        return impl.sizeFunc;
    }

    LOTVariant() = default;
    ~LOTVariant() noexcept {Destroy();}
    LOTVariant(const LOTVariant& other) { Copy(other);}
    LOTVariant(LOTVariant&& other) noexcept { Move(std::move(other));}
    LOTVariant& operator=(LOTVariant&& other) { Destroy(); Move(std::move(other)); return *this;}
    LOTVariant& operator=(const LOTVariant& other) { Destroy(); Copy(other); return *this;}
private:
    template <typename T>
    void construct(T& member, const T& val)
    {
        new (&member) T(val);
    }

    template <typename T>
    void moveConstruct(T& member, T&& val)
    {
        new (&member) T(std::move(val));
    }

    void Move(LOTVariant&& other)
    {
        switch (other.mTag) {
        case Type::Value:
        moveConstruct(impl.valueFunc, std::move(other.impl.valueFunc));
        break;
        case Type::Color:
        moveConstruct(impl.colorFunc, std::move(other.impl.colorFunc));
        break;
        case Type::Point:
        moveConstruct(impl.pointFunc, std::move(other.impl.pointFunc));
        break;
        case Type::Size:
        moveConstruct(impl.sizeFunc, std::move(other.impl.sizeFunc));
        break;
        default:
        break;
        }
        mTag = other.mTag;
        mPropery = other.mPropery;
        other.mTag = MonoState;
    }

    void Copy(const LOTVariant& other)
    {
        switch (other.mTag) {
        case Type::Value:
        construct(impl.valueFunc, other.impl.valueFunc);
        break;
        case Type::Color:
        construct(impl.colorFunc, other.impl.colorFunc);
        break;
        case Type::Point:
        construct(impl.pointFunc, other.impl.pointFunc);
        break;
        case Type::Size:
        construct(impl.sizeFunc, other.impl.sizeFunc);
        break;
        default:
        break;
        }
        mTag = other.mTag;
        mPropery = other.mPropery;
    }

    void Destroy()
    {
        switch(mTag) {
        case MonoState: {
            break;
        }
        case Value: {
            impl.valueFunc.~ValueFunc();
            break;
        }
        case Color: {
            impl.colorFunc.~ColorFunc();
            break;
        }
        case Point: {
            impl.pointFunc.~PointFunc();
            break;
        }
        case Size: {
            impl.sizeFunc.~SizeFunc();
            break;
        }
        }
    }

    enum Type {MonoState, Value, Color, Point , Size};
    imlottie::Property mPropery;
    Type              mTag{MonoState};
    union details{
        ColorFunc   colorFunc;
        ValueFunc   valueFunc;
        PointFunc   pointFunc;
        SizeFunc    sizeFunc;
        details(){}
        ~details(){}
    }impl;
};

class LOTFilter
{
public:
    void addValue(LOTVariant &value)
    {
        uint index = static_cast<uint>(value.property());
        if (mBitset.test(index)) {
            std::replace_if(mFilters.begin(),
                            mFilters.end(),
                            [&value](const LOTVariant &e) {return e.property() == value.property();},
                            value);
        } else {
            mBitset.set(index);
            mFilters.push_back(value);
        }
    }

    void removeValue(LOTVariant &value)
    {
        uint index = static_cast<uint>(value.property());
        if (mBitset.test(index)) {
            mBitset.reset(index);
            mFilters.erase(std::remove_if(mFilters.begin(),
                           mFilters.end(),
                           [&value](const LOTVariant &e) {return e.property() == value.property();}),
                           mFilters.end());
        }
    }
    bool hasFilter(imlottie::Property prop) const
    {
        return mBitset.test(static_cast<uint>(prop));
    }
    LottieColor color(imlottie::Property prop, int frame) const
    {
        imlottie::FrameInfo info(frame);
        imlottie::Color col = data(prop).color()(info);
        return LottieColor(col.r(), col.g(), col.b());
    }
    VPointF point(imlottie::Property prop, int frame) const
    {
        imlottie::FrameInfo info(frame);
        imlottie::Point pt = data(prop).point()(info);
        return VPointF(pt.x(), pt.y());
    }
    VSize scale(imlottie::Property prop, int frame) const
    {
        imlottie::FrameInfo info(frame);
        imlottie::Size sz = data(prop).size()(info);
        return VSize(sz.w(), sz.h());
    }
    float opacity(imlottie::Property prop, int frame) const
    {
        imlottie::FrameInfo info(frame);
        float val = data(prop).value()(info);
        return val/100;
    }
    float value(imlottie::Property prop, int frame) const
    {
        imlottie::FrameInfo info(frame);
        return data(prop).value()(info);
    }
private:
    const LOTVariant& data(Property prop) const
    {
        auto result = std::find_if(mFilters.begin(),
                                   mFilters.end(),
                                   [prop](const LOTVariant &e){return e.property() == prop;});
        return *result;
    }
    std::bitset<32>            mBitset{0};
    std::vector<LOTVariant>    mFilters;
};

template<typename T>
class LOTKeyFrame
{
public:
    float progress(int frameNo) const {
        return mInterpolator ? mInterpolator->value((frameNo - mStartFrame) / (mEndFrame - mStartFrame)) : 0;
    }
    T value(int frameNo) const {
        return mValue.value(progress(frameNo));
    }
    float angle(int frameNo) const {
        return mValue.angle(progress(frameNo));
    }

public:
    float                 mStartFrame{0};
    float                 mEndFrame{0};
    VInterpolator        *mInterpolator{nullptr};
    LOTKeyFrameValue<T>   mValue;
};


template<typename T>
class LOTAnimInfo
{
public:
    T value(int frameNo) const {
        if (mKeyFrames.front().mStartFrame >= frameNo)
            return mKeyFrames.front().mValue.mStartValue;
        if(mKeyFrames.back().mEndFrame <= frameNo)
            return mKeyFrames.back().mValue.mEndValue;

        for(const auto &keyFrame : mKeyFrames) {
            if (frameNo >= keyFrame.mStartFrame && frameNo < keyFrame.mEndFrame)
                return keyFrame.value(frameNo);
        }
        return T();
    }

    float angle(int frameNo) const {
        if ((mKeyFrames.front().mStartFrame >= frameNo) ||
            (mKeyFrames.back().mEndFrame <= frameNo) )
            return 0;

        for(const auto &keyFrame : mKeyFrames) {
            if (frameNo >= keyFrame.mStartFrame && frameNo < keyFrame.mEndFrame)
                return keyFrame.angle(frameNo);
        }
        return 0;
    }

    bool changed(int prevFrame, int curFrame) const {
        auto first = mKeyFrames.front().mStartFrame;
        auto last = mKeyFrames.back().mEndFrame;

        return !((first > prevFrame  && first > curFrame) ||
                 (last < prevFrame  && last < curFrame));
    }

public:
    std::vector<LOTKeyFrame<T>>    mKeyFrames;
};

template<typename T>
class LOTAnimatable
{
public:
    LOTAnimatable() { construct(impl.mValue, {}); }
    explicit LOTAnimatable(T value) { construct(impl.mValue, std::move(value)); }

    const LOTAnimInfo<T>& animation() const {return *(impl.mAnimInfo.get());}
    const T& value() const {return impl.mValue;}

    LOTAnimInfo<T>& animation()
    {
        if (mStatic) {
            destroy();
            construct(impl.mAnimInfo, std::make_unique<LOTAnimInfo<T>>());
            mStatic = false;
        }
        return *(impl.mAnimInfo.get());
    }

    T& value()
    {
        assert(mStatic);
        return impl.mValue;
    }

    LOTAnimatable(LOTAnimatable &&other) noexcept {
        if (!other.mStatic) {
            construct(impl.mAnimInfo, std::move(other.impl.mAnimInfo));
            mStatic = false;
        } else {
            construct(impl.mValue, std::move(other.impl.mValue));
            mStatic = true;
        }
    }
    // delete special member functions
    LOTAnimatable(const LOTAnimatable &) = delete;
    LOTAnimatable& operator=(const LOTAnimatable&) = delete;
    LOTAnimatable& operator=(LOTAnimatable&&) = delete;

    ~LOTAnimatable() {destroy();}

    bool isStatic() const {return mStatic;}

    T value(int frameNo) const {
        return isStatic() ? value() : animation().value(frameNo);
    }

    float angle(int frameNo) const {
        return isStatic() ? 0 : animation().angle(frameNo);
    }

    bool changed(int prevFrame, int curFrame) const {
        return isStatic() ? false : animation().changed(prevFrame, curFrame);
    }
private:
    template <typename Tp>
    void construct(Tp& member, Tp&& val)
    {
        new (&member) Tp(std::move(val));
    }

    void destroy() {
        if (mStatic) {
            impl.mValue.~T();
        } else {
            using std::unique_ptr;
            impl.mAnimInfo.~unique_ptr<LOTAnimInfo<T>>();
        }
    }
    union details {
        std::unique_ptr<LOTAnimInfo<T>>   mAnimInfo;
        T                                 mValue;
        details(){};
        details(const details&) = delete;
        details(details&&) = delete;
        details& operator=(details&&) = delete;
        details& operator=(const details&) = delete;
        ~details(){};
    }impl;
    bool                                 mStatic{true};
};


template <typename T>
class LOTProxyModel
{
public:
    LOTProxyModel(T *model): _modelData(model) {}
    LOTFilter& filter() {return mFilter;}
    const char* name() const {return _modelData->name();}
    LottieColor color(int frame) const
    {
        if (mFilter.hasFilter(imlottie::Property::StrokeColor)) {
            return mFilter.color(imlottie::Property::StrokeColor, frame);
        }
        return _modelData->color(frame);
    }
    float opacity(int frame) const
    {
        if (mFilter.hasFilter(imlottie::Property::StrokeOpacity)) {
            return mFilter.opacity(imlottie::Property::StrokeOpacity, frame);
        }
        return _modelData->opacity(frame);
    }
    float strokeWidth(int frame) const
    {
        if (mFilter.hasFilter(imlottie::Property::StrokeWidth)) {
            return mFilter.value(imlottie::Property::StrokeWidth, frame);
        }
        return _modelData->strokeWidth(frame);
    }
    float miterLimit() const {return _modelData->miterLimit();}
    CapStyle capStyle() const {return _modelData->capStyle();}
    JoinStyle joinStyle() const {return _modelData->joinStyle();}
    bool hasDashInfo() const { return _modelData->hasDashInfo();}
    void getDashInfo(int frameNo, std::vector<float>& result) const {
        return _modelData->getDashInfo(frameNo, result);
    }

private:
    T                         *_modelData;
    LOTFilter                  mFilter;
};

class LOTDataVisitor;
class LOTData
{
public:
    enum class Type :unsigned char {
        Composition = 1,
        Layer,
        ShapeGroup,
        Transform,
        Fill,
        Stroke,
        GFill,
        GStroke,
        Rect,
        Ellipse,
        Shape,
        Polystar,
        Trim,
        Repeater
    };

    explicit LOTData(LOTData::Type type):mPtr(nullptr)
    {
        mData._type = type;
        mData._static = true;
        mData._shortString = true;
        mData._hidden = false;
    }
    ~LOTData() { if (!shortString() && mPtr) free(mPtr); }
    LOTData(const LOTData&) = delete;
    LOTData& operator =(const LOTData&) = delete;

    void setStatic(bool value) { mData._static = value;}
    bool isStatic() const {return mData._static;}
    bool hidden() const {return mData._hidden;}
    void setHidden(bool value) {mData._hidden = value;}
    void setType(LOTData::Type type) {mData._type = type;}
    LOTData::Type type() const { return mData._type;}
    void setName(const char *name)
    {
        if (name) {
            auto len = strlen(name);
            if (len < maxShortStringLength) {
                setShortString(true);
                strncpy ( mData._buffer, name, len+1);
            } else {
                setShortString(false);
                mPtr = strdup(name);
            }

        }
    }
    const char* name() const {return shortString() ? mData._buffer : mPtr;}
private:
    static constexpr unsigned char maxShortStringLength = 14;
    void setShortString(bool value) {mData._shortString = value;}
    bool shortString() const {return mData._shortString;}
    struct Data{
        char           _buffer[maxShortStringLength];
        LOTData::Type  _type;
        bool           _static      : 1;
        bool           _hidden      : 1;
        bool           _shortString : 1;
    };
    union {
        Data  mData;
        char *mPtr{nullptr};
    };
};

class LOTFillData : public LOTData
{
public:
    LOTFillData():LOTData(LOTData::Type::Fill){}
    LottieColor color(int frameNo) const {return mColor.value(frameNo);}
    float opacity(int frameNo) const {return mOpacity.value(frameNo)/100.0f;}
    FillRule fillRule() const {return mFillRule;}
public:
    FillRule                       mFillRule{FillRule::Winding}; /* "r" */
    bool                           mEnabled{true}; /* "fillEnabled" */
    LOTAnimatable<LottieColor>     mColor;   /* "c" */
    LOTAnimatable<float>           mOpacity{100};  /* "o" */
};

template <>
class LOTProxyModel<LOTFillData>
{
public:
    LOTProxyModel(LOTFillData *model): _modelData(model) {}
    LOTFilter& filter() {return mFilter;}
    const char* name() const {return _modelData->name();}
    LottieColor color(int frame) const
    {
        if (mFilter.hasFilter(imlottie::Property::FillColor)) {
            return mFilter.color(imlottie::Property::FillColor, frame);
        }
        return _modelData->color(frame);
    }
    float opacity(int frame) const
    {
        if (mFilter.hasFilter(imlottie::Property::FillOpacity)) {
            return mFilter.opacity(imlottie::Property::FillOpacity, frame);
        }
        return _modelData->opacity(frame);
    }
    FillRule fillRule() const {return _modelData->fillRule();}
private:
    LOTFillData               *_modelData;
    LOTFilter                  mFilter;
};

class LOTGroupData: public LOTData
{
public:
    explicit LOTGroupData(LOTData::Type  type):LOTData(type){}
public:
    std::vector<LOTData *>  mChildren;
    LOTTransformData       *mTransform{nullptr};
};

struct TransformDataExtra
{
    LOTAnimatable<float>     m3DRx{0};
    LOTAnimatable<float>     m3DRy{0};
    LOTAnimatable<float>     m3DRz{0};
    LOTAnimatable<float>     mSeparateX{0};
    LOTAnimatable<float>     mSeparateY{0};
    bool                     mSeparate{false};
    bool                     m3DData{false};
};

struct TransformData
{
    VMatrix matrix(int frameNo, bool autoOrient = false) const;
    float opacity(int frameNo) const { return mOpacity.value(frameNo)/100.0f; }
    void createExtraData()
    {
        if (!mExtra) mExtra = std::make_unique<TransformDataExtra>();
    }
    LOTAnimatable<float>                   mRotation{0};  /* "r" */
    LOTAnimatable<VPointF>                 mScale{{100, 100}};     /* "s" */
    LOTAnimatable<VPointF>                 mPosition;  /* "p" */
    LOTAnimatable<VPointF>                 mAnchor;    /* "a" */
    LOTAnimatable<float>                   mOpacity{100};   /* "o" */
    std::unique_ptr<TransformDataExtra>    mExtra;
};

class LOTTransformData : public LOTData
{
public:
    LOTTransformData():LOTData(LOTData::Type::Transform){}
    void set(TransformData* data, bool staticFlag)
    {
        setStatic(staticFlag);
        if (isStatic()) {
            new (&impl.mStaticData) static_data(data->matrix(0), data->opacity(0));
        } else {
            impl.mData = data;
        }
    }
    VMatrix matrix(int frameNo, bool autoOrient = false) const
    {
        if (isStatic()) return impl.mStaticData.mMatrix;
        return impl.mData->matrix(frameNo, autoOrient);
    }
    float opacity(int frameNo) const
    {
        if (isStatic()) return impl.mStaticData.mOpacity;
        return impl.mData->opacity(frameNo);
    }
    LOTTransformData(const LOTTransformData&) = delete;
    LOTTransformData(LOTTransformData&&) = delete;
    LOTTransformData& operator=(LOTTransformData&) = delete;
    LOTTransformData& operator=(LOTTransformData&&) = delete;
    ~LOTTransformData() {destroy();}

private:
    void destroy() {
        if (isStatic()) {
            impl.mStaticData.~static_data();
        }
    }
    struct static_data {
        static_data(VMatrix &&m, float opacity):
            mOpacity(opacity), mMatrix(std::move(m)){}
        float    mOpacity;
        VMatrix  mMatrix;
    };
    union details {
        TransformData     *mData{nullptr};
        static_data        mStaticData;
        details(){};
        details(const details&) = delete;
        details(details&&) = delete;
        details& operator=(details&&) = delete;
        details& operator=(const details&) = delete;
        ~details(){};
    }impl;
};

template <>
class LOTProxyModel<LOTGroupData>
{
public:
    LOTProxyModel(LOTGroupData *model = nullptr): _modelData(model) {}
    bool hasModel() const { return _modelData ? true : false; }
    LOTFilter& filter() {return mFilter;}
    const char* name() const {return _modelData->name();}
    LOTTransformData* transform() const { return _modelData->mTransform; }
    VMatrix matrix(int frame) const {
        VMatrix mS, mR, mT;
        if (mFilter.hasFilter(imlottie::Property::TrScale)) {
            VSize s = mFilter.scale(imlottie::Property::TrScale, frame);
            mS.scale(s.width() / 100.0f, s.height() / 100.0f);
        }
        if (mFilter.hasFilter(imlottie::Property::TrRotation)) {
            mR.rotate(mFilter.value(imlottie::Property::TrRotation, frame));
        }
        if (mFilter.hasFilter(imlottie::Property::TrPosition)) {
            mT.translate(mFilter.point(imlottie::Property::TrPosition, frame));
        }

        return _modelData->mTransform->matrix(frame) * mS * mR * mT;
    }
private:
    LOTGroupData               *_modelData;
    LOTFilter                  mFilter;
};

using Marker = std::tuple<std::string, int , int>;
using LayerInfo = Marker;
enum class LottieBlendMode: uchar
{
    Normal = 0,
    Multiply = 1,
    Screen = 2,
    OverLay = 3
};

class LOTLayerData;
struct LOTAsset
{
    enum class Type : unsigned char{
        Precomp,
        Image,
        Char
    };
    bool isStatic() const {return mStatic;}
    void setStatic(bool value) {mStatic = value;}
    VBitmap  bitmap() const {return mBitmap;}
    void loadImageData(std::string data);
    void loadImagePath(std::string Path);
    Type                                      mAssetType{Type::Precomp};
    bool                                      mStatic{true};
    std::string                               mRefId; // ref id
    std::vector<LOTData *>                    mLayers;
    // image asset data
    int                                       mWidth{0};
    int                                       mHeight{0};
    VBitmap                                   mBitmap;
};

class LottieShapeData
{
public:
    void reserve(size_t size) {
        mPoints.reserve(mPoints.size() + size);
    }
    static void lerp(const LottieShapeData& start, const LottieShapeData& end, float t, VPath& result)
    {
        result.reset();
        auto size = std::min(start.mPoints.size(), end.mPoints.size());
        /* reserve exact memory requirement at once
        * ptSize = size + 1(size + close)
        * elmSize = size/3 cubic + 1 move + 1 close
        */
        result.reserve(size + 1 , size/3 + 2);
        result.moveTo(start.mPoints[0] + t * (end.mPoints[0] - start.mPoints[0]));
        for (size_t i = 1 ; i < size; i+=3) {
            result.cubicTo(start.mPoints[i] + t * (end.mPoints[i] - start.mPoints[i]),
                           start.mPoints[i+1] + t * (end.mPoints[i+1] - start.mPoints[i+1]),
                           start.mPoints[i+2] + t * (end.mPoints[i+2] - start.mPoints[i+2]));
        }
        if (start.mClosed) result.close();
    }
    void toPath(VPath& path) const {
        path.reset();

        if (mPoints.empty()) return;

        auto size = mPoints.size();
        auto points = mPoints.data();
        /* reserve exact memory requirement at once
        * ptSize = size + 1(size + close)
        * elmSize = size/3 cubic + 1 move + 1 close
        */
        path.reserve(size + 1 , size/3 + 2);
        path.moveTo(points[0]);
        for (size_t i = 1 ; i < size; i+=3) {
            path.cubicTo(points[i], points[i+1], points[i+2]);
        }
        if (mClosed)
            path.close();
    }
public:
    std::vector<VPointF> mPoints;
    bool                 mClosed = false;   /* "c" */
};

class LOTAnimatableShape : public LOTAnimatable<LottieShapeData>
{
public:
    void updatePath(int frameNo, VPath &path) const {
        if (isStatic()) {
            value().toPath(path);
        } else {
            const auto &vec = animation().mKeyFrames;
            if (vec.front().mStartFrame >= frameNo)
                return vec.front().mValue.mStartValue.toPath(path);
            if(vec.back().mEndFrame <= frameNo)
                return vec.back().mValue.mEndValue.toPath(path);

            for(const auto &keyFrame : vec) {
                if (frameNo >= keyFrame.mStartFrame && frameNo < keyFrame.mEndFrame) {
                    LottieShapeData::lerp(keyFrame.mValue.mStartValue,
                                          keyFrame.mValue.mEndValue,
                                          keyFrame.progress(frameNo),
                                          path);
                }
            }
        }
    }
};

class LOTMaskData
{
public:
    enum class Mode {
        None,
        Add,
        Substarct,
        Intersect,
        Difference
    };
    float opacity(int frameNo) const {return mOpacity.value(frameNo)/100.0f;}
    bool isStatic() const {return mIsStatic;}
public:
    LOTAnimatableShape                mShape;
    LOTAnimatable<float>              mOpacity{100};
    bool                              mInv{false};
    bool                              mIsStatic{true};
    LOTMaskData::Mode                 mMode;
};

class LOTCompositionData;
struct ExtraLayerData
{
    LottieColor                mSolidColor;
    std::string                mPreCompRefId;
    LOTAnimatable<float>       mTimeRemap;  /* "tm" */
    LOTCompositionData        *mCompRef{nullptr};
    LOTAsset                  *mAsset{nullptr};
    std::vector<LOTMaskData *>  mMasks;
};

class LOTLayerData : public LOTGroupData
{
public:
    LOTLayerData():LOTGroupData(LOTData::Type::Layer){}
    bool hasPathOperator() const noexcept {return mHasPathOperator;}
    bool hasGradient() const noexcept {return mHasGradient;}
    bool hasMask() const noexcept {return mHasMask;}
    bool hasRepeater() const noexcept {return mHasRepeater;}
    int id() const noexcept{ return mId;}
    int parentId() const noexcept{ return mParentId;}
    bool hasParent() const noexcept {return mParentId != -1;}
    int inFrame() const noexcept{return mInFrame;}
    int outFrame() const noexcept{return mOutFrame;}
    int startFrame() const noexcept{return mStartFrame;}
    LottieColor solidColor() const noexcept{return mExtra->mSolidColor;}
    bool autoOrient() const noexcept{return mAutoOrient;}
    int timeRemap(int frameNo) const;
    VSize layerSize() const {return mLayerSize;}
    bool precompLayer() const {return mLayerType == LayerType::Precomp;}
    VMatrix matrix(int frameNo) const
    {
        return mTransform ? mTransform->matrix(frameNo, autoOrient()) : VMatrix{};
    }
    float opacity(int frameNo) const
    {
        return mTransform ? mTransform->opacity(frameNo) : 1.0f;
    }
    LOTAsset* asset() const
    {
        return (mExtra && mExtra->mAsset) ? mExtra->mAsset : nullptr;
    }
public:
    ExtraLayerData* extra()
    {
        if (!mExtra) mExtra = std::make_unique<ExtraLayerData>();
        return mExtra.get();
    }
    MatteType            mMatteType{MatteType::None};
    LayerType            mLayerType{LayerType::Null};
    LottieBlendMode      mBlendMode{LottieBlendMode::Normal};
    bool                 mHasPathOperator{false};
    bool                 mHasMask{false};
    bool                 mHasRepeater{false};
    bool                 mHasGradient{false};
    bool                 mAutoOrient{false};
    VSize                mLayerSize;
    int                  mParentId{-1}; // Lottie the id of the parent in the composition
    int                  mId{-1};  // Lottie the group id  used for parenting.
    float                mTimeStreatch{1.0f};
    int                  mInFrame{0};
    int                  mOutFrame{0};
    int                  mStartFrame{0};
    std::unique_ptr<ExtraLayerData> mExtra{nullptr};
};

class LOTCompositionData : public LOTData
{
public:
    LOTCompositionData():LOTData(LOTData::Type::Composition){}
    std::vector<LayerInfo> layerInfoList() const;
    const std::vector<Marker> &markers() const { return  mMarkers;}
    double duration() const {
        return frameDuration() / frameRate(); // in second
    }
    size_t frameAtPos(double pos) const {
        if (pos < 0) pos = 0;
        if (pos > 1) pos = 1;
        return size_t(pos * frameDuration());
    }
    long frameAtTime(double timeInSec) const {
        return long(frameAtPos(timeInSec / duration()));
    }
    size_t totalFrame() const {return mEndFrame - mStartFrame;}
    long frameDuration() const {return mEndFrame - mStartFrame -1;}
    float frameRate() const {return mFrameRate;}
    long startFrame() const {return mStartFrame;}
    long endFrame() const {return mEndFrame;}
    VSize size() const {return mSize;}
    void processRepeaterObjects();
    void updateStats();
public:
    std::string          mVersion;
    VSize                mSize;
    long                 mStartFrame{0};
    long                 mEndFrame{0};
    float                mFrameRate{60};
    LottieBlendMode      mBlendMode{LottieBlendMode::Normal};
    LOTLayerData        *mRootLayer{nullptr};
    std::unordered_map<std::string,
        LOTAsset*>    mAssets;

    std::vector<Marker>     mMarkers;
    VArenaAlloc             mArenaAlloc{2048};
    LOTModelStat            mStats;
};

class LOTModel
{
public:
    bool  isStatic() const {return mRoot->isStatic();}
    VSize size() const {return mRoot->size();}
    double duration() const {return mRoot->duration();}
    size_t totalFrame() const {return mRoot->totalFrame();}
    size_t frameDuration() const {return mRoot->frameDuration();}
    double frameRate() const {return mRoot->frameRate();}
    size_t startFrame() const {return mRoot->startFrame();}
    size_t endFrame() const {return mRoot->endFrame();}
    size_t frameAtPos(double pos) const {return mRoot->frameAtPos(pos);}
    std::vector<LayerInfo> layerInfoList() const { return mRoot->layerInfoList();}
    const std::vector<Marker> &markers() const { return mRoot->markers();}
public:
    std::shared_ptr<LOTCompositionData> mRoot;
};

class LottieParserImpl;
class LottieParser {
public:
    ~LottieParser();
    LottieParser(char* str, const char *dir_path);
    std::shared_ptr<LOTModel> model();
private:
    std::unique_ptr<LottieParserImpl>  d;
};

inline LottieColor operator-(const LottieColor &c1, const LottieColor &c2)
{
    return LottieColor(c1.r - c2.r, c1.g - c2.g, c1.b - c2.b);
}
inline LottieColor operator+(const LottieColor &c1, const LottieColor &c2)
{
    return LottieColor(c1.r + c2.r, c1.g + c2.g, c1.b + c2.b);
}

inline const LottieColor operator*(const LottieColor &c, float m)
{ return LottieColor(c.r*m, c.g*m, c.b*m); }

inline const LottieColor operator*(float m, const LottieColor &c)
{ return LottieColor(c.r*m, c.g*m, c.b*m); }


template <>
struct LOTKeyFrameValue<VPointF>
{
    VPointF mStartValue;
    VPointF mEndValue;
    VPointF mInTangent;
    VPointF mOutTangent;
    bool    mPathKeyFrame = false;

    VPointF value(float t) const {
        if (mPathKeyFrame) {
            /*
            * position along the path calcualated
            * using bezier at progress length (t * bezlen)
            */
            VBezier b = VBezier::fromPoints(mStartValue, mStartValue + mOutTangent,
                                            mEndValue + mInTangent, mEndValue);
            return b.pointAt(b.tAtLength(t * b.length()));

        }
        return lerp(mStartValue, mEndValue, t);
    }

    float angle(float t) const {
        if (mPathKeyFrame) {
            VBezier b = VBezier::fromPoints(mStartValue, mStartValue + mOutTangent,
                                            mEndValue + mInTangent, mEndValue);
            return b.angleAt(b.tAtLength(t * b.length()));
        }
        return 0;
    }
};

class LOTShapeGroupData : public LOTGroupData
{
public:
    LOTShapeGroupData():LOTGroupData(LOTData::Type::ShapeGroup){}
};

/**
* TimeRemap has the value in time domain(in sec)
* To get the proper mapping first we get the mapped time at the current frame Number
* then we need to convert mapped time to frame number using the composition time line
* Ex: at frame 10 the mappend time is 0.5(500 ms) which will be convert to frame number
* 30 if the frame rate is 60. or will result to frame number 15 if the frame rate is 30.
*/
inline int LOTLayerData::timeRemap(int frameNo) const
{
    /*
    * only consider startFrame() when there is no timeRemap.
    * when a layer has timeremap bodymovin updates the startFrame()
    * of all child layer so we don't have to take care of it.
    */
    if (!mExtra || mExtra->mTimeRemap.isStatic())
        frameNo = frameNo - startFrame();
    else
        frameNo = mExtra->mCompRef->frameAtTime(mExtra->mTimeRemap.value(frameNo));
    /* Apply time streatch if it has any.
    * Time streatch is just a factor by which the animation will speedup or slow
    * down with respect to the overal animation.
    * Time streach factor is already applied to the layers inFrame and outFrame.
    * @TODO need to find out if timestreatch also affects the in and out frame of the
    * child layers or not. */
    return int(frameNo / mTimeStreatch);
}

struct LOTDashProperty
{
    std::vector<LOTAnimatable<float>> mData;
    bool empty() const {return mData.empty();}
    size_t size() const {return mData.size();}
    bool isStatic() const {
        for(const auto &elm : mData)
            if (!elm.isStatic()) return false;
        return true;
    }
    void getDashInfo(int frameNo, std::vector<float>& result) const;
};

class LOTStrokeData : public LOTData
{
public:
    LOTStrokeData():LOTData(LOTData::Type::Stroke){}
    LottieColor color(int frameNo) const {return mColor.value(frameNo);}
    float opacity(int frameNo) const {return mOpacity.value(frameNo)/100.0f;}
    float strokeWidth(int frameNo) const {return mWidth.value(frameNo);}
    CapStyle capStyle() const {return mCapStyle;}
    JoinStyle joinStyle() const {return mJoinStyle;}
    float miterLimit() const{return mMiterLimit;}
    bool  hasDashInfo() const {return !mDash.empty();}
    void getDashInfo(int frameNo, std::vector<float>& result) const
    {
        return mDash.getDashInfo(frameNo, result);
    }
public:
    LOTAnimatable<LottieColor>        mColor;      /* "c" */
    LOTAnimatable<float>              mOpacity{100};    /* "o" */
    LOTAnimatable<float>              mWidth{0};      /* "w" */
    CapStyle                          mCapStyle{CapStyle::Flat};   /* "lc" */
    JoinStyle                         mJoinStyle{JoinStyle::Miter};  /* "lj" */
    float                             mMiterLimit{0}; /* "ml" */
    LOTDashProperty                   mDash;
    bool                              mEnabled{true}; /* "fillEnabled" */
};

class LottieGradient
{
public:
    friend inline LottieGradient operator+(const LottieGradient &g1, const LottieGradient &g2);
    friend inline LottieGradient operator-(const LottieGradient &g1, const LottieGradient &g2);
    friend inline LottieGradient operator*(float m, const LottieGradient &g);
public:
    std::vector<float>    mGradient;
};

inline LottieGradient operator+(const LottieGradient &g1, const LottieGradient &g2)
{
    if (g1.mGradient.size() != g2.mGradient.size())
        return g1;

    LottieGradient newG;
    newG.mGradient = g1.mGradient;

    auto g2It = g2.mGradient.begin();
    for(auto &i : newG.mGradient) {
        i = i + *g2It;
        g2It++;
    }

    return newG;
}

inline LottieGradient operator-(const LottieGradient &g1, const LottieGradient &g2)
{
    if (g1.mGradient.size() != g2.mGradient.size())
        return g1;
    LottieGradient newG;
    newG.mGradient = g1.mGradient;

    auto g2It = g2.mGradient.begin();
    for(auto &i : newG.mGradient) {
        i = i - *g2It;
        g2It++;
    }

    return newG;
}

inline LottieGradient operator*(float m, const LottieGradient &g)
{
    LottieGradient newG;
    newG.mGradient = g.mGradient;

    for(auto &i : newG.mGradient) {
        i = i * m;
    }
    return newG;
}



class LOTGradient : public LOTData
{
public:
    explicit LOTGradient(LOTData::Type  type):LOTData(type){}
    inline float opacity(int frameNo) const {return mOpacity.value(frameNo)/100.0f;}
    void update(std::unique_ptr<VGradient> &grad, int frameNo);

private:
    void populate(VGradientStops &stops, int frameNo);
public:
    int                                 mGradientType{1};        /* "t" Linear=1 , Radial = 2*/
    LOTAnimatable<VPointF>              mStartPoint;          /* "s" */
    LOTAnimatable<VPointF>              mEndPoint;            /* "e" */
    LOTAnimatable<float>                mHighlightLength{0};     /* "h" */
    LOTAnimatable<float>                mHighlightAngle{0};      /* "a" */
    LOTAnimatable<float>                mOpacity{100};             /* "o" */
    LOTAnimatable<LottieGradient>       mGradient;            /* "g" */
    int                                 mColorPoints{-1};
    bool                                mEnabled{true};      /* "fillEnabled" */
};

class LOTGFillData : public LOTGradient
{
public:
    LOTGFillData():LOTGradient(LOTData::Type::GFill){}
    FillRule fillRule() const {return mFillRule;}
public:
    FillRule                       mFillRule{FillRule::Winding}; /* "r" */
};

class LOTGStrokeData : public LOTGradient
{
public:
    LOTGStrokeData():LOTGradient(LOTData::Type::GStroke){}
    float width(int frameNo) const {return mWidth.value(frameNo);}
    CapStyle capStyle() const {return mCapStyle;}
    JoinStyle joinStyle() const {return mJoinStyle;}
    float miterLimit() const{return mMiterLimit;}
    bool  hasDashInfo() const {return !mDash.empty();}
    void getDashInfo(int frameNo, std::vector<float>& result) const
    {
        return mDash.getDashInfo(frameNo, result);
    }
public:
    LOTAnimatable<float>           mWidth;       /* "w" */
    CapStyle                       mCapStyle{CapStyle::Flat};    /* "lc" */
    JoinStyle                      mJoinStyle{JoinStyle::Miter};   /* "lj" */
    float                          mMiterLimit{0};  /* "ml" */
    LOTDashProperty                mDash;
};

class LOTPath : public LOTData
{
public:
    explicit LOTPath(LOTData::Type  type):LOTData(type){}
    VPath::Direction direction() {
        return (mDirection == 3) ?
            VPath::Direction::CCW : VPath::Direction::CW;
    }
public:
    int                                    mDirection{1};
};

class LOTShapeData : public LOTPath
{
public:
    LOTShapeData():LOTPath(LOTData::Type::Shape){}
public:
    LOTAnimatableShape    mShape;
};

class LOTRectData : public LOTPath
{
public:
    LOTRectData():LOTPath(LOTData::Type::Rect){}
public:
    LOTAnimatable<VPointF>    mPos;
    LOTAnimatable<VPointF>    mSize;
    LOTAnimatable<float>      mRound{0};
};

class LOTEllipseData : public LOTPath
{
public:
    LOTEllipseData():LOTPath(LOTData::Type::Ellipse){}
public:
    LOTAnimatable<VPointF>   mPos;
    LOTAnimatable<VPointF>   mSize;
};

class LOTPolystarData : public LOTPath
{
public:
    enum class PolyType {
        Star = 1,
        Polygon = 2
    };
    LOTPolystarData():LOTPath(LOTData::Type::Polystar){}
public:
    LOTPolystarData::PolyType     mPolyType{PolyType::Polygon};
    LOTAnimatable<VPointF>        mPos;
    LOTAnimatable<float>          mPointCount{0};
    LOTAnimatable<float>          mInnerRadius{0};
    LOTAnimatable<float>          mOuterRadius{0};
    LOTAnimatable<float>          mInnerRoundness{0};
    LOTAnimatable<float>          mOuterRoundness{0};
    LOTAnimatable<float>          mRotation{0};
};

class LOTTrimData : public LOTData
{
public:
    struct Segment {
        float start{0};
        float end{0};
        Segment() = default;
        explicit Segment(float s, float e):start(s), end(e) {}
    };
    enum class TrimType {
        Simultaneously,
        Individually
    };
    LOTTrimData():LOTData(LOTData::Type::Trim){}
    /*
    * if start > end vector trims the path as a loop ( 2 segment)
    * if start < end vector trims the path without loop ( 1 segment).
    * if no offset then there is no loop.
    */
    Segment segment(int frameNo) const {
        float start = mStart.value(frameNo)/100.0f;
        float end = mEnd.value(frameNo)/100.0f;
        float offset = std::fmod(mOffset.value(frameNo), 360.0f)/ 360.0f;

        float diff = std::abs(start - end);
        if (vCompare(diff, 0.0f)) return Segment(0, 0);
        if (vCompare(diff, 1.0f)) return Segment(0, 1);

        if (offset > 0) {
            start += offset;
            end += offset;
            if (start <= 1 && end <=1) {
                return noloop(start, end);
            } else if (start > 1 && end > 1) {
                return noloop(start - 1, end - 1);
            } else {
                return (start > 1) ?
                    loop(start - 1 , end) : loop(start , end - 1);
            }
        } else {
            start += offset;
            end   += offset;
            if (start >= 0 && end >= 0) {
                return noloop(start, end);
            } else if (start < 0 && end < 0) {
                return noloop(1 + start, 1 + end);
            } else {
                return (start < 0) ?
                    loop(1 + start, end) : loop(start , 1 + end);
            }
        }
    }
    LOTTrimData::TrimType type() const {return mTrimType;}
private:
    Segment noloop(float start, float end) const{
        assert(start >= 0);
        assert(end >= 0);
        Segment s;
        s.start = std::min(start, end);
        s.end = std::max(start, end);
        return s;
    }
    Segment loop(float start, float end) const{
        assert(start >= 0);
        assert(end >= 0);
        Segment s;
        s.start = std::max(start, end);
        s.end = std::min(start, end);
        return s;
    }
public:
    LOTAnimatable<float>             mStart{0};
    LOTAnimatable<float>             mEnd{0};
    LOTAnimatable<float>             mOffset{0};
    LOTTrimData::TrimType            mTrimType{TrimType::Simultaneously};
};

class LOTRepeaterTransform
{
public:
    VMatrix matrix(int frameNo, float multiplier) const;
    float startOpacity(int frameNo) const { return mStartOpacity.value(frameNo)/100;}
    float endOpacity(int frameNo) const { return mEndOpacity.value(frameNo)/100;}
    bool isStatic() const
    {
        return mRotation.isStatic() &&
            mScale.isStatic() &&
            mPosition.isStatic() &&
            mAnchor.isStatic() &&
            mStartOpacity.isStatic() &&
            mEndOpacity.isStatic();
    }
public:
    LOTAnimatable<float>          mRotation{0};  /* "r" */
    LOTAnimatable<VPointF>        mScale{{100, 100}};     /* "s" */
    LOTAnimatable<VPointF>        mPosition;  /* "p" */
    LOTAnimatable<VPointF>        mAnchor;    /* "a" */
    LOTAnimatable<float>          mStartOpacity{100}; /* "so" */
    LOTAnimatable<float>          mEndOpacity{100};   /* "eo" */
};

class LOTRepeaterData : public LOTData
{
public:
    LOTRepeaterData():LOTData(LOTData::Type::Repeater){}
    LOTShapeGroupData *content() const { return mContent ? mContent : nullptr; }
    void setContent(LOTShapeGroupData *content) {mContent = content;}
    int maxCopies() const { return int(mMaxCopies);}
    float copies(int frameNo) const {return mCopies.value(frameNo);}
    float offset(int frameNo) const {return mOffset.value(frameNo);}
    bool processed() const {return mProcessed;}
    void markProcessed() {mProcessed = true;}
public:
    LOTShapeGroupData*                      mContent{nullptr};
    LOTRepeaterTransform                    mTransform;
    LOTAnimatable<float>                    mCopies{0};
    LOTAnimatable<float>                    mOffset{0};
    float                                   mMaxCopies{0.0};
    bool                                    mProcessed{false};
};

class LottieLoader
{
public:
    static void configureModelCacheSize(size_t cacheSize);
    bool load(const std::string &filePath, bool cachePolicy);
    bool loadFromData(std::string &&jsonData, const std::string &key,
                      const std::string &resourcePath, bool cachePolicy);
    std::shared_ptr<LOTModel> model();
private:
    std::shared_ptr<LOTModel>    mModel;
};

enum class DirtyFlagBit : uchar
{
    None   = 0x00,
    Matrix = 0x01,
    Alpha  = 0x02,
    All    = (Matrix | Alpha)
};

class LOTLayerItem;
class LOTMaskItem;
class VDrawable;
struct LOTNode;
struct LOTLayerNode;
class Surface;

class LOTDrawable : public VDrawable
{
public:
    void sync();
public:
    std::unique_ptr<LOTNode>  mCNode{nullptr};

    ~LOTDrawable();
};

class LOTCompItem
{
public:
    explicit LOTCompItem(LOTModel *model);
    bool update(int frameNo, const VSize &size, bool keepAspectRatio);
    VSize size() const { return mViewSize;}
    void buildRenderTree();
    const LOTLayerNode * renderTree()const;
    bool render(const Surface &surface);
    void setValue(const std::string &keypath, LOTVariant &value);
private:
    VBitmap                                     mSurface;
    VMatrix                                     mScaleMatrix;
    VSize                                       mViewSize;
    LOTCompositionData                         *mCompData{nullptr};
    LOTLayerItem                               *mRootLayer{nullptr};
    VArenaAlloc                                 mAllocator{2048};
    int                                         mCurFrameNo;
    bool                                        mKeepAspectRatio{true};
};

class LOTLayerMaskItem;

class LOTClipperItem
{
public:
    explicit LOTClipperItem(VSize size): mSize(size){}
    void update(const VMatrix &matrix);
    void preprocess(const VRect &clip);
    VRle rle(const VRle& mask);
public:
    VSize                    mSize;
    VPath                    mPath;
    VRle                     mMaskedRle;
    VRasterizer              mRasterizer;
    bool                     mRasterRequest{false};
};

typedef vFlag<DirtyFlagBit> DirtyFlag;
struct LOTNode;
struct LOTCApiData
{
    LOTCApiData();
    LOTLayerNode                  mLayer;
    std::vector<LOTMask>          mMasks;
    std::vector<LOTLayerNode *>   mLayers;
    std::vector<LOTNode *>        mCNodeList;
};

template< class T>
class VSpan
{
public:
    using reference         = T &;
    using pointer           = T *;
    using const_pointer     = T const *;
    using const_reference   = T const &;
    using index_type        = size_t;

    using iterator          = pointer;
    using const_iterator    = const_pointer;

    VSpan() = default;
    VSpan(pointer data, index_type size):_data(data), _size(size){}

    constexpr pointer data() const noexcept {return _data; }
    constexpr index_type size() const noexcept {return _size; }
    constexpr bool empty() const noexcept { return size() == 0 ;}
    constexpr iterator begin() const noexcept { return data(); }
    constexpr iterator end() const noexcept {return data() + size() ;}
    constexpr const_iterator cbegin() const noexcept {return  data();}
    constexpr const_iterator cend() const noexcept { return data() + size();}
    constexpr reference operator[]( index_type idx ) const { return *( data() + idx );}

private:
    pointer      _data{nullptr};
    index_type   _size{0};
};

using DrawableList = VSpan<VDrawable *>;
struct LOTLayerNode;
class LOTLayerItem
{
public:
    virtual ~LOTLayerItem() = default;
    LOTLayerItem& operator=(LOTLayerItem&&) noexcept = delete;
    LOTLayerItem(LOTLayerData *layerData);
    int id() const {return mLayerData->id();}
    int parentId() const {return mLayerData->parentId();}
    void setParentLayer(LOTLayerItem *parent){mParentLayer = parent;}
    void setComplexContent(bool value) { mComplexContent = value;}
    bool complexContent() const {return mComplexContent;}
    virtual void update(int frameNo, const VMatrix &parentMatrix, float parentAlpha);
    VMatrix matrix(int frameNo) const;
    void preprocess(const VRect& clip);
    virtual DrawableList renderList(){ return {};}
    virtual void render(VPainter *painter, const VRle &mask, const VRle &matteRle);
    bool hasMatte() { if (mLayerData->mMatteType == MatteType::None) return false; return true; }
    MatteType matteType() const { return mLayerData->mMatteType;}
    bool visible() const;
    virtual void buildLayerNode();
    LOTLayerNode& clayer() {return mCApiData->mLayer;}
    std::vector<LOTLayerNode *>& clayers() {return mCApiData->mLayers;}
    std::vector<LOTMask>& cmasks() {return mCApiData->mMasks;}
    std::vector<LOTNode *>& cnodes() {return mCApiData->mCNodeList;}
    const char* name() const {return mLayerData->name();}
    virtual bool resolveKeyPath(LOTKeyPath &keyPath, uint depth, LOTVariant &value);
    VBitmap& bitmap() {return mRenderBuffer;}
protected:
    virtual void preprocessStage(const VRect& clip) = 0;
    virtual void updateContent() = 0;
    inline VMatrix combinedMatrix() const {return mCombinedMatrix;}
    inline int frameNo() const {return mFrameNo;}
    inline float combinedAlpha() const {return mCombinedAlpha;}
    inline bool isStatic() const {return mLayerData->isStatic();}
    float opacity(int frameNo) const {return mLayerData->opacity(frameNo);}
    inline DirtyFlag flag() const {return mDirtyFlag;}
    bool skipRendering() const {return (!visible() || vIsZero(combinedAlpha()));}
protected:
    std::unique_ptr<LOTLayerMaskItem>           mLayerMask;
    LOTLayerData                               *mLayerData{nullptr};
    LOTLayerItem                               *mParentLayer{nullptr};
    VMatrix                                     mCombinedMatrix;
    VBitmap                                     mRenderBuffer;
    float                                       mCombinedAlpha{0.0};
    int                                         mFrameNo{-1};
    DirtyFlag                                   mDirtyFlag{DirtyFlagBit::All};
    bool                                        mComplexContent{false};
    std::unique_ptr<LOTCApiData>                mCApiData;
};

class LOTCompLayerItem: public LOTLayerItem
{
public:
    explicit LOTCompLayerItem(LOTLayerData *layerData, VArenaAlloc* allocator);

    void render(VPainter *painter, const VRle &mask, const VRle &matteRle) final;
    void buildLayerNode() final;
    bool resolveKeyPath(LOTKeyPath &keyPath, uint depth, LOTVariant &value) override;
protected:
    void preprocessStage(const VRect& clip) final;
    void updateContent() final;
private:
    void renderHelper(VPainter *painter, const VRle &mask, const VRle &matteRle);
    void renderMatteLayer(VPainter *painter, const VRle &inheritMask, const VRle &matteRle,
                          LOTLayerItem *layer, LOTLayerItem *src);
private:
    std::vector<LOTLayerItem*>            mLayers;
    std::unique_ptr<LOTClipperItem>       mClipper;
};

class LOTSolidLayerItem: public LOTLayerItem
{
public:
    explicit LOTSolidLayerItem(LOTLayerData *layerData);
    void buildLayerNode() final;
    DrawableList renderList() final;
protected:
    void preprocessStage(const VRect& clip) final;
    void updateContent() final;
private:
    LOTDrawable                  mRenderNode;
    VDrawable                   *mDrawableList{nullptr}; //to work with the Span api
};

class LOTContentItem;
class LOTContentGroupItem;
class LOTShapeLayerItem: public LOTLayerItem
{
public:
    explicit LOTShapeLayerItem(LOTLayerData *layerData, VArenaAlloc* allocator);
    DrawableList renderList() final;
    void buildLayerNode() final;
    bool resolveKeyPath(LOTKeyPath &keyPath, uint depth, LOTVariant &value) override;
protected:
    void preprocessStage(const VRect& clip) final;
    void updateContent() final;
    std::vector<VDrawable *>             mDrawableList;
    LOTContentGroupItem                 *mRoot{nullptr};
};

class LOTNullLayerItem: public LOTLayerItem
{
public:
    explicit LOTNullLayerItem(LOTLayerData *layerData);
protected:
    void preprocessStage(const VRect&) final {}
    void updateContent() final;
};

class LOTImageLayerItem: public LOTLayerItem
{
public:
    explicit LOTImageLayerItem(LOTLayerData *layerData);
    void buildLayerNode() final;
    DrawableList renderList() final;
protected:
    void preprocessStage(const VRect& clip) final;
    void updateContent() final;
private:
    LOTDrawable                  mRenderNode;
    VTexture                     mTexture;
    VDrawable                   *mDrawableList{nullptr}; //to work with the Span api
};

class LOTMaskItem
{
public:
    explicit LOTMaskItem(LOTMaskData *data): mData(data){}
    void update(int frameNo, const VMatrix &parentMatrix, float parentAlpha, const DirtyFlag &flag);
    LOTMaskData::Mode maskMode() const { return mData->mMode;}
    VRle rle();
    void preprocess(const VRect &clip);
public:
    LOTMaskData             *mData{nullptr};
    VPath                    mLocalPath;
    VPath                    mFinalPath;
    VRasterizer              mRasterizer;
    float                    mCombinedAlpha{0};
    bool                     mRasterRequest{false};
};

/*
* Handels mask property of a layer item
*/
class LOTLayerMaskItem
{
public:
    explicit LOTLayerMaskItem(LOTLayerData *layerData);
    void update(int frameNo, const VMatrix &parentMatrix, float parentAlpha, const DirtyFlag &flag);
    bool isStatic() const {return mStatic;}
    VRle maskRle(const VRect &clipRect);
    void preprocess(const VRect &clip);
public:
    std::vector<LOTMaskItem>   mMasks;
    VRle                       mRle;
    bool                       mStatic{true};
    bool                       mDirty{true};
};

class LOTPathDataItem;
class LOTPaintDataItem;
class LOTTrimItem;

enum class ContentType : uchar
{
    Unknown,
    Group,
    Path,
    Paint,
    Trim
};

class LOTContentGroupItem;
class LOTContentItem
{
public:
    virtual ~LOTContentItem() = default;
    LOTContentItem& operator=(LOTContentItem&&) noexcept = delete;
    virtual void update(int frameNo, const VMatrix &parentMatrix, float parentAlpha, const DirtyFlag &flag) = 0;   virtual void renderList(std::vector<VDrawable *> &){}
    virtual bool resolveKeyPath(LOTKeyPath &, uint, LOTVariant &) {return false;}
    virtual ContentType type() const {return ContentType::Unknown;}
};

class LOTContentGroupItem: public LOTContentItem
{
public:
    LOTContentGroupItem() = default;
    explicit LOTContentGroupItem(LOTGroupData *data, VArenaAlloc* allocator);
    void addChildren(LOTGroupData *data, VArenaAlloc* allocator);
    void update(int frameNo, const VMatrix &parentMatrix, float parentAlpha, const DirtyFlag &flag) override;
    void applyTrim();
    void processTrimItems(std::vector<LOTPathDataItem *> &list);
    void processPaintItems(std::vector<LOTPathDataItem *> &list);
    void renderList(std::vector<VDrawable *> &list) override;
    ContentType type() const final {return ContentType::Group;}
    const VMatrix & matrix() const { return mMatrix;}
    const char* name() const
    {
        static const char* TAG = "__";
        return mModel.hasModel() ? mModel.name() : TAG;
    }
    bool resolveKeyPath(LOTKeyPath &keyPath, uint depth, LOTVariant &value) override;
protected:
    std::vector<LOTContentItem*>   mContents;
    VMatrix                                        mMatrix;
private:
    LOTProxyModel<LOTGroupData> mModel;
};

class LOTPathDataItem : public LOTContentItem
{
public:
    LOTPathDataItem(bool staticPath): mStaticPath(staticPath){}
    void update(int frameNo, const VMatrix &parentMatrix, float parentAlpha, const DirtyFlag &flag) final;
    ContentType type() const final {return ContentType::Path;}
    bool dirty() const {return mDirtyPath;}
    const VPath &localPath() const {return mTemp;}
    void finalPath(VPath& result);
    void updatePath(const VPath &path) {mTemp = path; mDirtyPath = true;}
    bool staticPath() const { return mStaticPath; }
    void setParent(LOTContentGroupItem *parent) {mParent = parent;}
    LOTContentGroupItem *parent() const {return mParent;}
protected:
    virtual void updatePath(VPath& path, int frameNo) = 0;
    virtual bool hasChanged(int prevFrame, int curFrame) = 0;
private:
    bool hasChanged(int frameNo) {
        int prevFrame = mFrameNo;
        mFrameNo = frameNo;
        if (prevFrame == -1) return true;
        if (mStaticPath ||
            (prevFrame == frameNo)) return false;
        return hasChanged(prevFrame, frameNo);
    }
    LOTContentGroupItem                    *mParent{nullptr};
    VPath                                   mLocalPath;
    VPath                                   mTemp;
    int                                     mFrameNo{-1};
    bool                                    mDirtyPath{true};
    bool                                    mStaticPath;
};

class LOTRectItem: public LOTPathDataItem
{
public:
    explicit LOTRectItem(LOTRectData *data);
protected:
    void updatePath(VPath& path, int frameNo) final;
    LOTRectData           *mData{nullptr};

    bool hasChanged(int prevFrame, int curFrame) final {
        return (mData->mPos.changed(prevFrame, curFrame) ||
                mData->mSize.changed(prevFrame, curFrame) ||
                mData->mRound.changed(prevFrame, curFrame));
    }
};

class LOTEllipseItem: public LOTPathDataItem
{
public:
    explicit LOTEllipseItem(LOTEllipseData *data);
private:
    void updatePath(VPath& path, int frameNo) final;
    LOTEllipseData           *mData{nullptr};
    bool hasChanged(int prevFrame, int curFrame) final {
        return (mData->mPos.changed(prevFrame, curFrame) ||
                mData->mSize.changed(prevFrame, curFrame));
    }
};

class LOTShapeItem: public LOTPathDataItem
{
public:
    explicit LOTShapeItem(LOTShapeData *data);
private:
    void updatePath(VPath& path, int frameNo) final;
    LOTShapeData             *mData{nullptr};
    bool hasChanged(int prevFrame, int curFrame) final {
        return mData->mShape.changed(prevFrame, curFrame);
    }
};

class LOTPolystarItem: public LOTPathDataItem
{
public:
    explicit LOTPolystarItem(LOTPolystarData *data);
private:
    void updatePath(VPath& path, int frameNo) final;
    LOTPolystarData             *mData{nullptr};

    bool hasChanged(int prevFrame, int curFrame) final {
        return (mData->mPos.changed(prevFrame, curFrame) ||
                mData->mPointCount.changed(prevFrame, curFrame) ||
                mData->mInnerRadius.changed(prevFrame, curFrame) ||
                mData->mOuterRadius.changed(prevFrame, curFrame) ||
                mData->mInnerRoundness.changed(prevFrame, curFrame) ||
                mData->mOuterRoundness.changed(prevFrame, curFrame) ||
                mData->mRotation.changed(prevFrame, curFrame));
    }
};



class LOTPaintDataItem : public LOTContentItem
{
public:
    LOTPaintDataItem(bool staticContent);
    void addPathItems(std::vector<LOTPathDataItem *> &list, size_t startOffset);
    void update(int frameNo, const VMatrix &parentMatrix, float parentAlpha, const DirtyFlag &flag) override;
    void renderList(std::vector<VDrawable *> &list) final;
    ContentType type() const final {return ContentType::Paint;}
protected:
    virtual bool updateContent(int frameNo, const VMatrix &matrix, float alpha) = 0;
private:
    void updateRenderNode();
protected:
    std::vector<LOTPathDataItem *>   mPathItems;
    LOTDrawable                      mDrawable;
    VPath                            mPath;
    DirtyFlag                        mFlag;
    bool                             mStaticContent;
    bool                             mRenderNodeUpdate{true};
    bool                             mContentToRender{true};
};

class LOTFillItem : public LOTPaintDataItem
{
public:
    explicit LOTFillItem(LOTFillData *data);
protected:
    bool updateContent(int frameNo, const VMatrix &matrix, float alpha) final;
    bool resolveKeyPath(LOTKeyPath &keyPath, uint depth, LOTVariant &value) final;
private:
    LOTProxyModel<LOTFillData> mModel;
};

class LOTGFillItem : public LOTPaintDataItem
{
public:
    explicit LOTGFillItem(LOTGFillData *data);
protected:
    bool updateContent(int frameNo, const VMatrix &matrix, float alpha) final;
private:
    LOTGFillData                 *mData{nullptr};
    std::unique_ptr<VGradient>    mGradient;
};

class LOTStrokeItem : public LOTPaintDataItem
{
public:
    explicit LOTStrokeItem(LOTStrokeData *data);
protected:
    bool updateContent(int frameNo, const VMatrix &matrix, float alpha) final;
    bool resolveKeyPath(LOTKeyPath &keyPath, uint depth, LOTVariant &value) final;
private:
    LOTProxyModel<LOTStrokeData> mModel;
};

class LOTGStrokeItem : public LOTPaintDataItem
{
public:
    explicit LOTGStrokeItem(LOTGStrokeData *data);
protected:
    bool updateContent(int frameNo, const VMatrix &matrix, float alpha) final;
private:
    LOTGStrokeData               *mData{nullptr};
    std::unique_ptr<VGradient>    mGradient;
};

class LOTTrimItem : public LOTContentItem
{
public:
    explicit LOTTrimItem(LOTTrimData *data);
    void update(int frameNo, const VMatrix &parentMatrix, float parentAlpha, const DirtyFlag &flag) final;
    ContentType type() const final {return ContentType::Trim;}
    void update();
    void addPathItems(std::vector<LOTPathDataItem *> &list, size_t startOffset);
private:
    bool pathDirty() const {
        for (auto &i : mPathItems) {
            if (i->dirty())
                return true;
        }
        return false;
    }
    struct Cache {
        int                     mFrameNo{-1};
        LOTTrimData::Segment    mSegment{};
    };
    Cache                            mCache;
    std::vector<LOTPathDataItem *>   mPathItems;
    LOTTrimData                     *mData{nullptr};
    VPathMesure                      mPathMesure;
    bool                             mDirty{true};
};

class LOTRepeaterItem : public LOTContentGroupItem
{
public:
    explicit LOTRepeaterItem(LOTRepeaterData *data, VArenaAlloc* allocator);
    void update(int frameNo, const VMatrix &parentMatrix, float parentAlpha, const DirtyFlag &flag) final;
    void renderList(std::vector<VDrawable *> &list) final;
private:
    LOTRepeaterData             *mRepeaterData{nullptr};
    bool                         mHidden{false};
    int                          mCopies{0};
};

enum LOTBrushType: uchar
{
    BrushSolid = 0,
    BrushGradient
};

enum LOTFillRule: uchar
{
    FillEvenOdd = 0,
    FillWinding
};

typedef enum
{
    JoinMiter = 0,
    JoinBevel,
    JoinRound
} LOTJoinStyle;

enum LOTCapStyle: uchar
{
    CapFlat = 0,
    CapSquare,
    CapRound
} ;

enum LOTGradientType: uchar
{
    GradientLinear = 0,
    GradientRadial
};

struct LOTGradientStop
{
    float         pos;
    unsigned char r, g, b, a;
};

struct LOTMarker {
    char *name;
    size_t startframe;
    size_t endframe;
};

struct LOTMarkerList {
    LOTMarker *ptr;
    size_t size;
};

typedef struct LOTNode {

#define ChangeFlagNone 0x0000
#define ChangeFlagPath 0x0001
#define ChangeFlagPaint 0x0010
#define ChangeFlagAll (ChangeFlagPath & ChangeFlagPaint)

    struct {
        const float *ptPtr;
        size_t       ptCount;
        const char  *elmPtr;
        size_t       elmCount;
    } mPath;

    struct {
        unsigned char r, g, b, a;
    } mColor;

    struct {
        unsigned char  enable;
        float       width;
        LOTCapStyle  cap;
        LOTJoinStyle join;
        float       miterLimit;
        float    *dashArray;
        int       dashArraySize;
    } mStroke;

    struct {
        LOTGradientType  type;
        LOTGradientStop *stopPtr;
        size_t           stopCount;
        struct {
            float x, y;
        } start, end, center, focal;
        float cradius;
        float fradius;
    } mGradient;

    struct {
        unsigned char *data;
        size_t width;
        size_t height;
        unsigned char mAlpha;
        struct {
            float m11; float m12; float m13;
            float m21; float m22; float m23;
            float m31; float m32; float m33;
        } mMatrix;
    } mImageInfo;

    int       mFlag;
    LOTBrushType mBrushType;
    LOTFillRule  mFillRule;

    const char  *keypath;
} LOTNode;

class AnimationImpl;
struct LOTNode;
struct LOTLayerNode;

struct Color_Type{};
struct Point_Type{};
struct Size_Type{};
struct Float_Type{};
template <typename T> struct MapType;

class Surface {
public:
    /**
    *  @brief Surface object constructor.
    *  @param[in] buffer surface buffer.
    *  @param[in] width  surface width.
    *  @param[in] height  surface height.
    *  @param[in] bytesPerLine  number of bytes in a surface scanline.
    *  @note Default surface format is ARGB32_Premultiplied.
    */
    Surface(uint32_t *buffer, size_t width, size_t height, size_t bytesPerLine);

    /**
    *  @brief Sets the Draw Area available on the Surface.
    *  Lottie will use the draw region size to generate frame image
    *  and will update only the draw rgion of the surface.
    *  @param[in] x      region area x position.
    *  @param[in] y      region area y position.
    *  @param[in] width  region area width.
    *  @param[in] height region area height.
    *  @note Default surface format is ARGB32_Premultiplied.
    *  @note Default draw region area is [ 0 , 0, surface width , surface height]
    */
    void setDrawRegion(size_t x, size_t y, size_t width, size_t height);

    /**
    *  @brief Returns width of the surface.
    *  @return surface width
    */
    size_t width() const {return mWidth;}

    /**
    *  @brief Returns height of the surface.
    *  @return surface height
    */
    size_t height() const {return mHeight;}

    /**
    *  @brief Returns number of bytes in the surface scanline.
    *  @return number of bytes in scanline.
    */
    size_t  bytesPerLine() const {return mBytesPerLine;}

    /**
    *  @brief Returns buffer attached tp the surface.
    *  @return buffer attaced to the Surface.
    */
    uint32_t *buffer() const {return mBuffer;}

    /**
    *  @brief Returns drawable area width of the surface.
    *  @return drawable area width
    *  @note Default value is width() of the surface
    */
    size_t drawRegionWidth() const {return mDrawArea.w;}

    /**
    *  @brief Returns drawable area height of the surface.
    *  @return drawable area height
    *  @note Default value is height() of the surface
    */
    size_t drawRegionHeight() const {return mDrawArea.h;}

    /**
    *  @brief Returns drawable area's x position of the surface.
    *  @return drawable area's x potition.
    *  @note Default value is 0
    */
    size_t drawRegionPosX() const {return mDrawArea.x;}

    /**
    *  @brief Returns drawable area's y position of the surface.
    *  @return drawable area's y potition.
    *  @note Default value is 0
    */
    size_t drawRegionPosY() const {return mDrawArea.y;}

    bool isNeedClear() const { return mNeedClear; }
    void setNeedClear(bool needClear) { mNeedClear = needClear; }

    Surface() = default;
private:
    uint32_t    *mBuffer{nullptr};
    size_t       mWidth{0};
    size_t       mHeight{0};
    size_t       mBytesPerLine{0};
    struct {
        size_t   x{0};
        size_t   y{0};
        size_t   w{0};
        size_t   h{0};
    }mDrawArea;
    bool mNeedClear{true};
};

using MarkerList = std::vector<std::tuple<std::string, int , int>>;
/**
*  @brief https://helpx.adobe.com/after-effects/using/layer-markers-composition-markers.html
*  Markers exported form AE are used to describe a segmnet of an animation {comment/tag , startFrame, endFrame}
*  Marker can be use to devide a resource in to separate animations by tagging the segment with comment string ,
*  start frame and duration of that segment.
*/

using LayerInfoList = std::vector<std::tuple<std::string, int , int>>;

class Animation {
public:

    /**
    *  @brief Constructs an animation object from file path.
    *
    *  @param[in] path Lottie resource file path
    *  @param[in] cachePolicy whether to cache or not the model data.
    *             use only when need to explicit disabl caching for a
    *             particular resource. To disable caching at library level
    *             use @see configureModelCacheSize() instead.
    *
    *  @return Animation object that can render the contents of the
    *          Lottie resource represented by file path.
    *
    *  @internal
    */
    static std::shared_ptr<Animation> loadFromFile(const std::string &path, bool cachePolicy=true);

    /**
    *  @brief Constructs an animation object from JSON string data.
    *
    *  @param[in] jsonData The JSON string data.
    *  @param[in] key the string that will be used to cache the JSON string data.
    *  @param[in] resourcePath the path will be used to search for external resource.
    *  @param[in] cachePolicy whether to cache or not the model data.
    *             use only when need to explicit disabl caching for a
    *             particular resource. To disable caching at library level
    *             use @see configureModelCacheSize() instead.
    *
    *  @return Animation object that can render the contents of the
    *          Lottie resource represented by JSON string data.
    *
    *  @internal
    */
    static std::shared_ptr<Animation> loadFromData(std::string jsonData, const std::string &key, const std::string &resourcePath="", bool cachePolicy=true);

    /**
    *  @brief Returns default framerate of the Lottie resource.
    *  @return framerate of the Lottie resource
    */
    double frameRate() const;

    /**
    *  @brief Returns total number of frames present in the Lottie resource.
    *  @return frame count of the Lottie resource.
    *  @note frame number starts with 0.
    *  @internal
    */
    size_t totalFrame() const;

    /**
    *  @brief Returns default viewport size of the Lottie resource.
    *  @param[out] width  default width of the viewport.
    *  @param[out] height default height of the viewport.
    */
    void   size(size_t &width, size_t &height) const;

    /**
    *  @brief Returns total animation duration of Lottie resource in second.
    *         it uses totalFrame() and frameRate() to calculate the duration.
    *         duration = totalFrame() / frameRate().
    *  @return total animation duration in second.
    *  @retval 0 if the Lottie resource has no animation.
    *  @see totalFrame()
    *  @see frameRate()
    */
    double duration() const;

    /**
    *  @brief Returns frame number for a given position.
    *         this function helps to map the position value retuned
    *         by the animator to a frame number in side the Lottie resource.
    *         frame_number = lerp(start_frame, endframe, pos);
    *  @param[in] pos normalized position value [0 ... 1]
    *  @return frame numer maps to the position value [startFrame .... endFrame]
    */
    size_t frameAtPos(double pos);

    /**
    *  @brief Renders the content to surface synchronously.
    *         for performance use the async rendering @see render
    *  @param[in] frameNo Content corresponds to the @p frameNo needs to be drawn
    *  @param[in] surface Surface in which content will be drawn
    *  @param[in] keepAspectRatio whether to keep the aspect ratio while scaling the content.
    */
    void              renderSync(size_t frameNo, Surface surface, bool keepAspectRatio=true);

    /**
    *  @brief Returns root layer of the composition updated with
    *         content of the Lottie resource at frame number @p frameNo.
    *  @param[in] frameNo Content corresponds to the @p frameNo needs to be extracted.
    *  @param[in] width   content viewbox width
    *  @param[in] height  content viewbox height
    *  @return Root layer node.
    */
    const LOTLayerNode * renderTree(size_t frameNo, size_t width, size_t height) const;

    /**
    *  @brief Returns Composition Markers.
    *  @return returns MarkerList of the Composition.
    *  @see MarkerList
    */
    const MarkerList& markers() const;

    /**
    *  @brief Returns Layer information{name, inFrame, outFrame} of all the child layers  of the composition.
    *  @return List of Layer Information of the Composition.
    *  @see LayerInfoList
    */
    const LayerInfoList& layers() const;

    /**
    *  @brief Sets property value for the specified {@link KeyPath}. This {@link KeyPath} can resolve
    *  to multiple contents. In that case, the callback's value will apply to all of them.
    *  Keypath should conatin object names separated by (.) and can handle globe(**) or wildchar(*).
    *  @usage
    *  To change fillcolor property of fill1 object in the layer1->group1->fill1 hirarchy to RED color
    *     player->setValue<imlottie::Property::FillColor>("layer1.group1.fill1", imlottie::Color(1, 0, 0);
    *  if all the color property inside group1 needs to be changed to GREEN color
    *     player->setValue<imlottie::Property::FillColor>("**.group1.**", imlottie::Color(0, 1, 0);
    */
    template<Property prop, typename AnyValue>
    void setValue(const std::string &keypath, AnyValue value)
    {
        setValue(MapType<std::integral_constant<Property, prop>>{}, prop, keypath, value);
    }

    ~Animation();
    Animation();

private:
    void setValue(Color_Type, Property, const std::string &, Color);
    void setValue(Float_Type, Property, const std::string &, float);
    void setValue(Size_Type, Property, const std::string &, Size);
    void setValue(Point_Type, Property, const std::string &, Point);

    void setValue(Color_Type, Property, const std::string &, std::function<Color(const FrameInfo &)> &&);
    void setValue(Float_Type, Property, const std::string &, std::function<float(const FrameInfo &)> &&);
    void setValue(Size_Type, Property, const std::string &, std::function<Size(const FrameInfo &)> &&);
    void setValue(Point_Type, Property, const std::string &, std::function<Point(const FrameInfo &)> &&);

    std::unique_ptr<AnimationImpl> d;
};

//Map Property to Value type
template<> struct MapType<std::integral_constant<Property, Property::FillColor>>: Color_Type{};
template<> struct MapType<std::integral_constant<Property, Property::StrokeColor>>: Color_Type{};
template<> struct MapType<std::integral_constant<Property, Property::FillOpacity>>: Float_Type{};
template<> struct MapType<std::integral_constant<Property, Property::StrokeOpacity>>: Float_Type{};
template<> struct MapType<std::integral_constant<Property, Property::StrokeWidth>>: Float_Type{};
template<> struct MapType<std::integral_constant<Property, Property::TrRotation>>: Float_Type{};
template<> struct MapType<std::integral_constant<Property, Property::TrOpacity>>: Float_Type{};
template<> struct MapType<std::integral_constant<Property, Property::TrAnchor>>: Point_Type{};
template<> struct MapType<std::integral_constant<Property, Property::TrPosition>>: Point_Type{};
template<> struct MapType<std::integral_constant<Property, Property::TrScale>>: Size_Type{};


} // end namespace imlottie
