#define MAXBUTWIDTHSP 4
#define MINSHIFTSP 1
#define MAXSHIFTSP 1
#define CONFIGMAX 4
#define ISAMAX 2

namespace sleef_internal {
void dft2f_0_purecsp(float *, const float *, const int);
void dft2b_0_purecsp(float *, const float *, const int);
void tbut2f_0_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void tbut2b_0_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but2f_0_purecsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but2b_0_purecsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but2f_0_0_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but2b_0_0_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void dft4f_0_purecsp(float *, const float *, const int);
void dft4b_0_purecsp(float *, const float *, const int);
void tbut4f_0_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void tbut4b_0_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but4f_0_purecsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but4b_0_purecsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but4f_0_0_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but4b_0_0_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void dft8f_0_purecsp(float *, const float *, const int);
void dft8b_0_purecsp(float *, const float *, const int);
void tbut8f_0_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void tbut8b_0_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but8f_0_purecsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but8b_0_purecsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but8f_0_0_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but8b_0_0_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void dft16f_0_purecsp(float *, const float *, const int);
void dft16b_0_purecsp(float *, const float *, const int);
void tbut16f_0_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void tbut16b_0_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but16f_0_purecsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but16b_0_purecsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but16f_0_0_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but16b_0_0_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void dft2f_2_purecsp(float *, const float *, const int);
void dft2b_2_purecsp(float *, const float *, const int);
void tbut2f_2_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void tbut2b_2_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but2f_2_purecsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but2b_2_purecsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but2f_0_2_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but2b_0_2_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void dft4f_2_purecsp(float *, const float *, const int);
void dft4b_2_purecsp(float *, const float *, const int);
void tbut4f_2_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void tbut4b_2_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but4f_2_purecsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but4b_2_purecsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but4f_0_2_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but4b_0_2_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void dft8f_2_purecsp(float *, const float *, const int);
void dft8b_2_purecsp(float *, const float *, const int);
void tbut8f_2_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void tbut8b_2_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but8f_2_purecsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but8b_2_purecsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but8f_0_2_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but8b_0_2_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void dft16f_2_purecsp(float *, const float *, const int);
void dft16b_2_purecsp(float *, const float *, const int);
void tbut16f_2_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void tbut16b_2_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but16f_2_purecsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but16b_2_purecsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but16f_0_2_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but16b_0_2_purecsp(float *, uint32_t *, const float *, const int, const float *, const int);
void realSub0_purecsp(float *, const float *, const int, const float *, const float *);
void realSub1_purecsp(float *, const float *, const int, const float *, const float *, const int);
int getInt_purecsp(int);
const void *getPtr_purecsp(int);
void dft2f_0_advsimdsp(float *, const float *, const int);
void dft2b_0_advsimdsp(float *, const float *, const int);
void tbut2f_0_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void tbut2b_0_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but2f_0_advsimdsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but2b_0_advsimdsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but2f_0_0_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but2b_0_0_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void dft4f_0_advsimdsp(float *, const float *, const int);
void dft4b_0_advsimdsp(float *, const float *, const int);
void tbut4f_0_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void tbut4b_0_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but4f_0_advsimdsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but4b_0_advsimdsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but4f_0_0_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but4b_0_0_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void dft8f_0_advsimdsp(float *, const float *, const int);
void dft8b_0_advsimdsp(float *, const float *, const int);
void tbut8f_0_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void tbut8b_0_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but8f_0_advsimdsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but8b_0_advsimdsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but8f_0_0_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but8b_0_0_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void dft16f_0_advsimdsp(float *, const float *, const int);
void dft16b_0_advsimdsp(float *, const float *, const int);
void tbut16f_0_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void tbut16b_0_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but16f_0_advsimdsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but16b_0_advsimdsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but16f_0_0_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but16b_0_0_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void dft2f_2_advsimdsp(float *, const float *, const int);
void dft2b_2_advsimdsp(float *, const float *, const int);
void tbut2f_2_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void tbut2b_2_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but2f_2_advsimdsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but2b_2_advsimdsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but2f_0_2_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but2b_0_2_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void dft4f_2_advsimdsp(float *, const float *, const int);
void dft4b_2_advsimdsp(float *, const float *, const int);
void tbut4f_2_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void tbut4b_2_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but4f_2_advsimdsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but4b_2_advsimdsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but4f_0_2_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but4b_0_2_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void dft8f_2_advsimdsp(float *, const float *, const int);
void dft8b_2_advsimdsp(float *, const float *, const int);
void tbut8f_2_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void tbut8b_2_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but8f_2_advsimdsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but8b_2_advsimdsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but8f_0_2_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but8b_0_2_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void dft16f_2_advsimdsp(float *, const float *, const int);
void dft16b_2_advsimdsp(float *, const float *, const int);
void tbut16f_2_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void tbut16b_2_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but16f_2_advsimdsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but16b_2_advsimdsp(float *, uint32_t *, const int, const float *, const int, const float *, const int);
void but16f_0_2_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void but16b_0_2_advsimdsp(float *, uint32_t *, const float *, const int, const float *, const int);
void realSub0_advsimdsp(float *, const float *, const int, const float *, const float *);
void realSub1_advsimdsp(float *, const float *, const int, const float *, const float *, const int);
int getInt_advsimdsp(int);
const void *getPtr_advsimdsp(int);

void (*dftf_float[CONFIGMAX][ISAMAX][MAXBUTWIDTHSP+1])(float *, const float *, const int) = {
  {
    {NULL, dft2f_0_purecsp, dft4f_0_purecsp, dft8f_0_purecsp, dft16f_0_purecsp, },
    {NULL, dft2f_0_advsimdsp, dft4f_0_advsimdsp, dft8f_0_advsimdsp, dft16f_0_advsimdsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, dft2f_2_purecsp, dft4f_2_purecsp, dft8f_2_purecsp, dft16f_2_purecsp, },
    {NULL, dft2f_2_advsimdsp, dft4f_2_advsimdsp, dft8f_2_advsimdsp, dft16f_2_advsimdsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*dftb_float[CONFIGMAX][ISAMAX][MAXBUTWIDTHSP+1])(float *, const float *, const int) = {
  {
    {NULL, dft2f_0_purecsp, dft4b_0_purecsp, dft8b_0_purecsp, dft16b_0_purecsp, },
    {NULL, dft2f_0_advsimdsp, dft4b_0_advsimdsp, dft8b_0_advsimdsp, dft16b_0_advsimdsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, dft2f_2_purecsp, dft4b_2_purecsp, dft8b_2_purecsp, dft16b_2_purecsp, },
    {NULL, dft2f_2_advsimdsp, dft4b_2_advsimdsp, dft8b_2_advsimdsp, dft16b_2_advsimdsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*tbutf_float[CONFIGMAX][ISAMAX][MAXBUTWIDTHSP+1])(float *, uint32_t *, const float *, const int, const float *, const int) = {
  {
    {NULL, tbut2f_0_purecsp, tbut4f_0_purecsp, tbut8f_0_purecsp, tbut16f_0_purecsp, },
    {NULL, tbut2f_0_advsimdsp, tbut4f_0_advsimdsp, tbut8f_0_advsimdsp, tbut16f_0_advsimdsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, tbut2f_2_purecsp, tbut4f_2_purecsp, tbut8f_2_purecsp, tbut16f_2_purecsp, },
    {NULL, tbut2f_2_advsimdsp, tbut4f_2_advsimdsp, tbut8f_2_advsimdsp, tbut16f_2_advsimdsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*tbutb_float[CONFIGMAX][ISAMAX][MAXBUTWIDTHSP+1])(float *, uint32_t *, const float *, const int, const float *, const int) = {
  {
    {NULL, tbut2b_0_purecsp, tbut4b_0_purecsp, tbut8b_0_purecsp, tbut16b_0_purecsp, },
    {NULL, tbut2b_0_advsimdsp, tbut4b_0_advsimdsp, tbut8b_0_advsimdsp, tbut16b_0_advsimdsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, tbut2b_2_purecsp, tbut4b_2_purecsp, tbut8b_2_purecsp, tbut16b_2_purecsp, },
    {NULL, tbut2b_2_advsimdsp, tbut4b_2_advsimdsp, tbut8b_2_advsimdsp, tbut16b_2_advsimdsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*butf_float[CONFIGMAX][ISAMAX][MAXBUTWIDTHSP+1])(float *, uint32_t *, const int, const float *, const int, const float *, const int) = {
  {
    {NULL, but2f_0_purecsp, but4f_0_purecsp, but8f_0_purecsp, but16f_0_purecsp, },
    {NULL, but2f_0_advsimdsp, but4f_0_advsimdsp, but8f_0_advsimdsp, but16f_0_advsimdsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, but2f_2_purecsp, but4f_2_purecsp, but8f_2_purecsp, but16f_2_purecsp, },
    {NULL, but2f_2_advsimdsp, but4f_2_advsimdsp, but8f_2_advsimdsp, but16f_2_advsimdsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*butb_float[CONFIGMAX][ISAMAX][MAXBUTWIDTHSP+1])(float *, uint32_t *, const int, const float *, const int, const float *, const int) = {
  {
    {NULL, but2b_0_purecsp, but4b_0_purecsp, but8b_0_purecsp, but16b_0_purecsp, },
    {NULL, but2b_0_advsimdsp, but4b_0_advsimdsp, but8b_0_advsimdsp, but16b_0_advsimdsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, but2b_2_purecsp, but4b_2_purecsp, but8b_2_purecsp, but16b_2_purecsp, },
    {NULL, but2b_2_advsimdsp, but4b_2_advsimdsp, but8b_2_advsimdsp, but16b_2_advsimdsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*tbutfs_float[MAXSHIFTSP][CONFIGMAX][ISAMAX][MAXBUTWIDTHSP+1])(float *, uint32_t *, const float *, const float *, const int) = {
  {
    {
      {NULL, NULL, NULL, NULL, NULL, },
      {NULL, NULL, NULL, NULL, NULL, },
    },
    {
      {NULL, NULL, NULL, NULL, NULL, },
      {NULL, NULL, NULL, NULL, NULL, },
    },
    {
      {NULL, NULL, NULL, NULL, NULL, },
      {NULL, NULL, NULL, NULL, NULL, },
    },
    {
      {NULL, NULL, NULL, NULL, NULL, },
      {NULL, NULL, NULL, NULL, NULL, },
    },
  },
};

void (*tbutbs_float[MAXSHIFTSP][CONFIGMAX][ISAMAX][MAXBUTWIDTHSP+1])(float *, uint32_t *, const float *, const float *, const int) = {
  {
    {
      {NULL, NULL, NULL, NULL, NULL, },
      {NULL, NULL, NULL, NULL, NULL, },
    },
    {
      {NULL, NULL, NULL, NULL, NULL, },
      {NULL, NULL, NULL, NULL, NULL, },
    },
    {
      {NULL, NULL, NULL, NULL, NULL, },
      {NULL, NULL, NULL, NULL, NULL, },
    },
    {
      {NULL, NULL, NULL, NULL, NULL, },
      {NULL, NULL, NULL, NULL, NULL, },
    },
  },
};

void (*realSub0_float[ISAMAX])(float *, const float *, const int, const float *, const float *) = {
  realSub0_purecsp, realSub0_advsimdsp, 
};

void (*realSub1_float[ISAMAX])(float *, const float *, const int, const float *, const float *, const int) = {
  realSub1_purecsp, realSub1_advsimdsp, 
};

int (*getInt_float[16])(int) = {
  getInt_purecsp, getInt_advsimdsp, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
};

const void *(*getPtr_float[16])(int) = {
  getPtr_purecsp, getPtr_advsimdsp, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
};

} // namespace sleef_internal
