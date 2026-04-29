#define MAXBUTWIDTHDP 4
#define MINSHIFTDP 1
#define MAXSHIFTDP 1
#define CONFIGMAX 4
#define ISAMAX 2

namespace sleef_internal {
void dft2f_0_purecdp(double *, const double *, const int);
void dft2b_0_purecdp(double *, const double *, const int);
void tbut2f_0_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void tbut2b_0_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void but2f_0_purecdp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but2b_0_purecdp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but2f_0_0_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void but2b_0_0_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void dft4f_0_purecdp(double *, const double *, const int);
void dft4b_0_purecdp(double *, const double *, const int);
void tbut4f_0_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void tbut4b_0_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void but4f_0_purecdp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but4b_0_purecdp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but4f_0_0_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void but4b_0_0_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void dft8f_0_purecdp(double *, const double *, const int);
void dft8b_0_purecdp(double *, const double *, const int);
void tbut8f_0_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void tbut8b_0_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void but8f_0_purecdp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but8b_0_purecdp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but8f_0_0_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void but8b_0_0_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void dft16f_0_purecdp(double *, const double *, const int);
void dft16b_0_purecdp(double *, const double *, const int);
void tbut16f_0_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void tbut16b_0_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void but16f_0_purecdp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but16b_0_purecdp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but16f_0_0_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void but16b_0_0_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void dft2f_2_purecdp(double *, const double *, const int);
void dft2b_2_purecdp(double *, const double *, const int);
void tbut2f_2_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void tbut2b_2_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void but2f_2_purecdp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but2b_2_purecdp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but2f_0_2_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void but2b_0_2_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void dft4f_2_purecdp(double *, const double *, const int);
void dft4b_2_purecdp(double *, const double *, const int);
void tbut4f_2_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void tbut4b_2_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void but4f_2_purecdp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but4b_2_purecdp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but4f_0_2_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void but4b_0_2_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void dft8f_2_purecdp(double *, const double *, const int);
void dft8b_2_purecdp(double *, const double *, const int);
void tbut8f_2_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void tbut8b_2_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void but8f_2_purecdp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but8b_2_purecdp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but8f_0_2_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void but8b_0_2_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void dft16f_2_purecdp(double *, const double *, const int);
void dft16b_2_purecdp(double *, const double *, const int);
void tbut16f_2_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void tbut16b_2_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void but16f_2_purecdp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but16b_2_purecdp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but16f_0_2_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void but16b_0_2_purecdp(double *, uint32_t *, const double *, const int, const double *, const int);
void realSub0_purecdp(double *, const double *, const int, const double *, const double *);
void realSub1_purecdp(double *, const double *, const int, const double *, const double *, const int);
int getInt_purecdp(int);
const void *getPtr_purecdp(int);
void dft2f_0_advsimddp(double *, const double *, const int);
void dft2b_0_advsimddp(double *, const double *, const int);
void tbut2f_0_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void tbut2b_0_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void but2f_0_advsimddp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but2b_0_advsimddp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but2f_0_0_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void but2b_0_0_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void dft4f_0_advsimddp(double *, const double *, const int);
void dft4b_0_advsimddp(double *, const double *, const int);
void tbut4f_0_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void tbut4b_0_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void but4f_0_advsimddp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but4b_0_advsimddp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but4f_0_0_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void but4b_0_0_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void dft8f_0_advsimddp(double *, const double *, const int);
void dft8b_0_advsimddp(double *, const double *, const int);
void tbut8f_0_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void tbut8b_0_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void but8f_0_advsimddp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but8b_0_advsimddp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but8f_0_0_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void but8b_0_0_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void dft16f_0_advsimddp(double *, const double *, const int);
void dft16b_0_advsimddp(double *, const double *, const int);
void tbut16f_0_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void tbut16b_0_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void but16f_0_advsimddp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but16b_0_advsimddp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but16f_0_0_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void but16b_0_0_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void dft2f_2_advsimddp(double *, const double *, const int);
void dft2b_2_advsimddp(double *, const double *, const int);
void tbut2f_2_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void tbut2b_2_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void but2f_2_advsimddp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but2b_2_advsimddp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but2f_0_2_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void but2b_0_2_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void dft4f_2_advsimddp(double *, const double *, const int);
void dft4b_2_advsimddp(double *, const double *, const int);
void tbut4f_2_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void tbut4b_2_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void but4f_2_advsimddp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but4b_2_advsimddp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but4f_0_2_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void but4b_0_2_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void dft8f_2_advsimddp(double *, const double *, const int);
void dft8b_2_advsimddp(double *, const double *, const int);
void tbut8f_2_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void tbut8b_2_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void but8f_2_advsimddp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but8b_2_advsimddp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but8f_0_2_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void but8b_0_2_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void dft16f_2_advsimddp(double *, const double *, const int);
void dft16b_2_advsimddp(double *, const double *, const int);
void tbut16f_2_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void tbut16b_2_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void but16f_2_advsimddp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but16b_2_advsimddp(double *, uint32_t *, const int, const double *, const int, const double *, const int);
void but16f_0_2_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void but16b_0_2_advsimddp(double *, uint32_t *, const double *, const int, const double *, const int);
void realSub0_advsimddp(double *, const double *, const int, const double *, const double *);
void realSub1_advsimddp(double *, const double *, const int, const double *, const double *, const int);
int getInt_advsimddp(int);
const void *getPtr_advsimddp(int);

void (*dftf_double[CONFIGMAX][ISAMAX][MAXBUTWIDTHDP+1])(double *, const double *, const int) = {
  {
    {NULL, dft2f_0_purecdp, dft4f_0_purecdp, dft8f_0_purecdp, dft16f_0_purecdp, },
    {NULL, dft2f_0_advsimddp, dft4f_0_advsimddp, dft8f_0_advsimddp, dft16f_0_advsimddp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, dft2f_2_purecdp, dft4f_2_purecdp, dft8f_2_purecdp, dft16f_2_purecdp, },
    {NULL, dft2f_2_advsimddp, dft4f_2_advsimddp, dft8f_2_advsimddp, dft16f_2_advsimddp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*dftb_double[CONFIGMAX][ISAMAX][MAXBUTWIDTHDP+1])(double *, const double *, const int) = {
  {
    {NULL, dft2f_0_purecdp, dft4b_0_purecdp, dft8b_0_purecdp, dft16b_0_purecdp, },
    {NULL, dft2f_0_advsimddp, dft4b_0_advsimddp, dft8b_0_advsimddp, dft16b_0_advsimddp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, dft2f_2_purecdp, dft4b_2_purecdp, dft8b_2_purecdp, dft16b_2_purecdp, },
    {NULL, dft2f_2_advsimddp, dft4b_2_advsimddp, dft8b_2_advsimddp, dft16b_2_advsimddp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*tbutf_double[CONFIGMAX][ISAMAX][MAXBUTWIDTHDP+1])(double *, uint32_t *, const double *, const int, const double *, const int) = {
  {
    {NULL, tbut2f_0_purecdp, tbut4f_0_purecdp, tbut8f_0_purecdp, tbut16f_0_purecdp, },
    {NULL, tbut2f_0_advsimddp, tbut4f_0_advsimddp, tbut8f_0_advsimddp, tbut16f_0_advsimddp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, tbut2f_2_purecdp, tbut4f_2_purecdp, tbut8f_2_purecdp, tbut16f_2_purecdp, },
    {NULL, tbut2f_2_advsimddp, tbut4f_2_advsimddp, tbut8f_2_advsimddp, tbut16f_2_advsimddp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*tbutb_double[CONFIGMAX][ISAMAX][MAXBUTWIDTHDP+1])(double *, uint32_t *, const double *, const int, const double *, const int) = {
  {
    {NULL, tbut2b_0_purecdp, tbut4b_0_purecdp, tbut8b_0_purecdp, tbut16b_0_purecdp, },
    {NULL, tbut2b_0_advsimddp, tbut4b_0_advsimddp, tbut8b_0_advsimddp, tbut16b_0_advsimddp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, tbut2b_2_purecdp, tbut4b_2_purecdp, tbut8b_2_purecdp, tbut16b_2_purecdp, },
    {NULL, tbut2b_2_advsimddp, tbut4b_2_advsimddp, tbut8b_2_advsimddp, tbut16b_2_advsimddp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*butf_double[CONFIGMAX][ISAMAX][MAXBUTWIDTHDP+1])(double *, uint32_t *, const int, const double *, const int, const double *, const int) = {
  {
    {NULL, but2f_0_purecdp, but4f_0_purecdp, but8f_0_purecdp, but16f_0_purecdp, },
    {NULL, but2f_0_advsimddp, but4f_0_advsimddp, but8f_0_advsimddp, but16f_0_advsimddp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, but2f_2_purecdp, but4f_2_purecdp, but8f_2_purecdp, but16f_2_purecdp, },
    {NULL, but2f_2_advsimddp, but4f_2_advsimddp, but8f_2_advsimddp, but16f_2_advsimddp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*butb_double[CONFIGMAX][ISAMAX][MAXBUTWIDTHDP+1])(double *, uint32_t *, const int, const double *, const int, const double *, const int) = {
  {
    {NULL, but2b_0_purecdp, but4b_0_purecdp, but8b_0_purecdp, but16b_0_purecdp, },
    {NULL, but2b_0_advsimddp, but4b_0_advsimddp, but8b_0_advsimddp, but16b_0_advsimddp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, but2b_2_purecdp, but4b_2_purecdp, but8b_2_purecdp, but16b_2_purecdp, },
    {NULL, but2b_2_advsimddp, but4b_2_advsimddp, but8b_2_advsimddp, but16b_2_advsimddp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*tbutfs_double[MAXSHIFTDP][CONFIGMAX][ISAMAX][MAXBUTWIDTHDP+1])(double *, uint32_t *, const double *, const double *, const int) = {
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

void (*tbutbs_double[MAXSHIFTDP][CONFIGMAX][ISAMAX][MAXBUTWIDTHDP+1])(double *, uint32_t *, const double *, const double *, const int) = {
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

void (*realSub0_double[ISAMAX])(double *, const double *, const int, const double *, const double *) = {
  realSub0_purecdp, realSub0_advsimddp, 
};

void (*realSub1_double[ISAMAX])(double *, const double *, const int, const double *, const double *, const int) = {
  realSub1_purecdp, realSub1_advsimddp, 
};

int (*getInt_double[16])(int) = {
  getInt_purecdp, getInt_advsimddp, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
};

const void *(*getPtr_double[16])(int) = {
  getPtr_purecdp, getPtr_advsimddp, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
};

} // namespace sleef_internal
