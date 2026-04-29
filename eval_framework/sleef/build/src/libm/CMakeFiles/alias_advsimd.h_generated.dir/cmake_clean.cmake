file(REMOVE_RECURSE
  "alias_ADVSIMD_dp.h.tmp"
  "alias_ADVSIMD_sp.h.tmp"
  "include/alias_advsimd.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/alias_advsimd.h_generated.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
