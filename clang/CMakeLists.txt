llvm_map_components_to_libnames(LLVM_LIBS support core)
# weried problem that if use `clang` rather than `libclang`
# link options would not contains libclang.so
set(CLANG_LIBS clang)


add_executable(diagnose diagnose.cc)
target_link_libraries(diagnose
    PRIVATE ${CLANG_LIBS} ${LLVM_LIBS}
)

add_executable(tokenizer tokenizer.cc)
target_link_libraries(tokenizer
    PRIVATE ${CLANG_LIBS} ${LLVM_LIBS}
)

add_executable(parser parser)
target_link_libraries(parser
    PRIVATE ${CLANG_LIBS} ${LLVM_LIBS}
)