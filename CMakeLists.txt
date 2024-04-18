add_library(imlottie STATIC
    imottie_renderer.cpp
    freetype/v_ft_math.cpp
    freetype/v_ft_raster.cpp
    freetype/v_ft_stroker.cpp
    )
set(IMLOTTIE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "imlottie source directory")
target_include_directories(imlottie PRIVATE ${IMLOTTIE_DIR}/freetype)
target_include_directories(imlottie PUBLIC ${IMLOTTIE_DIR})
target_link_libraries(imlottie PRIVATE rapidjson imgui)
