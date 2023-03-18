#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>
#include "pictures/p1.h"

#define SIDE 2

static int w, h;  // Screen size

#define KEYNAME(key) \
  [AM_KEY_##key] = #key,
static const char *key_names[] = { AM_KEYS(KEYNAME) };

static inline void puts(const char *s) {
  for (; *s; s++) putch(*s);
}

void print_key() {
  AM_INPUT_KEYBRD_T event = { .keycode = AM_KEY_NONE };
  ioe_read(AM_INPUT_KEYBRD, &event);
  if (event.keycode != AM_KEY_NONE && event.keydown) {
    puts("Key pressed: ");
    puts(key_names[event.keycode]);
    puts("\n");
  }
}

static void draw_tile(int x, int y, int w, int h, uint32_t color) {
  uint32_t pixels[w * h]; // WARNING: large stack-allocated memory
  AM_GPU_FBDRAW_T event = {
    .x = x, .y = y, .w = w, .h = h, .sync = 1,
    .pixels = pixels,
  };
  for (int i = 0; i < w * h; i++) {
    pixels[i] = color;
  }
  ioe_write(AM_GPU_FBDRAW, &event);
}

void splash() {
  AM_GPU_CONFIG_T info = {0};
  ioe_read(AM_GPU_CONFIG, &info);
  w = info.width;
  h = info.height;
  printf("w:%d\th:%d\n", w, h);
  int i = 0;
  for (int x = 0; x * SIDE <= h; x ++) {
    for (int y = 0; y * SIDE <= w; y++) {
      if(i + 2 > p1_webp_len){
        printf("don't fit\n");
        return;
      }
      unsigned red = p1_webp[i], green = p1_webp[i + 1], blue = p1_webp[i + 2];
      i += 3;
      unsigned rgb = (red << 16) | (green << 8) | blue;
      // if ((x & 1) ^ (y & 1)) {
        draw_tile(y * SIDE, x * SIDE, SIDE, SIDE, rgb); // white
      // }
    }
  }
}

// Operating system is a C program!
int main(const char *args) {
  ioe_init();

  puts("mainargs = \"");
  puts(args);  // make run mainargs=xxx
  puts("\"\n");

  splash();

  puts("Press any key to see its key code...\n");
  while (1) {
    print_key();
  }
  return 0;
}
