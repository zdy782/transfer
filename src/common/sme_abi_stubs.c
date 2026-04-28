/*
 * Weak fallback SME ABI routines for standalone benchmark binaries.
 * A full platform/compiler-rt implementation overrides these definitions.
 */
__attribute__((weak, visibility("default")))
void __arm_tpidr2_save(void) __arm_streaming_compatible {
}

__attribute__((weak, visibility("default")))
void __arm_tpidr2_restore(void) __arm_streaming_compatible {
}

__attribute__((weak, visibility("default")))
void __arm_za_disable(void) __arm_streaming_compatible {
}
