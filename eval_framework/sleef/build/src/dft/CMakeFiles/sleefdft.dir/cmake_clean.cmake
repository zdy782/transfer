file(REMOVE_RECURSE
  "../../lib/libsleefdft.a"
  "../../lib/libsleefdft.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/sleefdft.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
