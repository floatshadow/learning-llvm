llvm_map_components_to_libnames(LLVM_LIBS support core)


add_executable(quick_start quick_start.cc)
target_link_libraries(quick_start 
    PRIVATE "${LLVM_LIBS}"
)