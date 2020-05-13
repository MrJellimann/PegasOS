void lfb_init();
void lfb_print(int x, int y, char *s);


void init_framebuffer();
void fbSetPixel(int x, int y, unsigned int colour);
void fbWriteChar(char character);
void fbWriteString(char* string);