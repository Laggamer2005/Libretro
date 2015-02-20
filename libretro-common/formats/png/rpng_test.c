/* Copyright  (C) 2010-2015 The RetroArch team
 *
 * ---------------------------------------------------------------------------------------
 * The following license statement only applies to this file (rpng_test.c).
 * ---------------------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <formats/rpng.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#ifdef HAVE_IMLIB2
#include <Imlib2.h>
#endif

static bool test_write_image(const char *path)
{
   const uint32_t test_data[] = {
      0xff000000 | 0x50, 0xff000000 | 0x80,
      0xff000000 | 0x40, 0xff000000 | 0x88,
      0xff000000 | 0x50, 0xff000000 | 0x80,
      0xff000000 | 0x40, 0xff000000 | 0x88,
      0xff000000 | 0xc3, 0xff000000 | 0xd3,
      0xff000000 | 0xc3, 0xff000000 | 0xd3,
      0xff000000 | 0xc3, 0xff000000 | 0xd3,
      0xff000000 | 0xc3, 0xff000000 | 0xd3,
   };

   if (!rpng_save_image_argb(path, test_data, 4, 4, 16))
      return false;
   return true;
}

static bool test_load_image(const char *path, uint32_t *data)
{
   unsigned width = 0;
   unsigned height = 0;

   if (!rpng_load_image_argb(path, &data, &width, &height))
      return false;

   fprintf(stderr, "Path: %s.\n", path);
   fprintf(stderr, "Got image: %u x %u.\n", width, height);

#if 0
   fprintf(stderr, "\nRPNG:\n");
   for (unsigned h = 0; h < height; h++)
   {
      unsigned w;
      for (w = 0; w < width; w++)
         fprintf(stderr, "[%08x] ", data[h * width + w]);
      fprintf(stderr, "\n");
   }
#endif

   return true;
}

static bool test_with_imlib(const char *in_path, uint32_t *data)
{
#ifdef HAVE_IMLIB2
   Imlib_Image img;
   unsigned width, height;
   const uint32_t *imlib_data = NULL;

   /* Validate with imlib2 as well. */
   img = imlib_load_image(in_path);
   if (!img)
      return false;

   imlib_context_set_image(img);

   width      = imlib_image_get_width();
   height     = imlib_image_get_width();
   imlib_data = imlib_image_get_data_for_reading_only();

#if 0
   fprintf(stderr, "\nImlib:\n");
   for (unsigned h = 0; h < height; h++)
   {
      for (unsigned w = 0; w < width; w++)
         fprintf(stderr, "[%08x] ", imlib_data[h * width + w]);
      fprintf(stderr, "\n");
   }
#endif

   if (memcmp(imlib_data, &data, width * height * sizeof(uint32_t)) != 0)
   {
      fprintf(stderr, "Imlib and RPNG differs!\n");
      return false;
   }
   else
      fprintf(stderr, "Imlib and RPNG are equivalent!\n");

   imlib_free_image();
#endif
   return true;
}

static int test_blocking_rpng(const char *path, uint32_t *data)
{
   if (!test_write_image("/tmp/test.png"))
      return 1;
   if (!test_load_image(path, data))
      return 2;

#ifdef HAVE_IMLIB2
   if (!test_with_imlib(path, data))
      return 5;
#endif

   return 0;
}

int main(int argc, char *argv[])
{
   int ret = 0;
   uint32_t *data = NULL;
   const char *in_path = "/tmp/test.png";

   if (argc > 2)
   {
      fprintf(stderr, "Usage: %s <png file>\n", argv[0]);
      return 1;
   }

   if (argc == 2)
      in_path = argv[1];

   ret = test_blocking_rpng(in_path, data);

   free(data);

   return ret;
}

