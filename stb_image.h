extern "C"
{
typedef unsigned char stbi_uc;
stbi_uc* stbi_load_from_file  (FILE *f,                  int *x, int *y, int *comp, int req_comp);
}
