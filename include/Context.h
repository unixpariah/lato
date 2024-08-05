#include "Character.h"
#include "math.h"

typedef enum {
  LATO_ENCODING_ASCII,
} LatoEncoding;

typedef enum {
  CHAR_DATA_ENCODING,
  CHAR_DATA_CHARACTERS,
} CharDataType;

typedef struct {
  CharDataType type;
  union {
    struct {
      LatoEncoding encoding;
    };
    struct {
      int *characters;
      int length;
    };
  } data;
} CharData;

typedef struct {
  const char *family;
  float size;
  float weight;
} Font;

typedef struct {
  Font font;
  CharData char_data;
  GLuint UBO;
} LatoContext;

void lato_context_set_characters(LatoContext *lato_context, int characters[]);

void lato_context_set_font_family(LatoContext *lato_context,
                                  const char *font_family);

void lato_context_set_font_size(LatoContext *lato_context, float size);

void lato_context_set_font_weight(LatoContext *lato_context, float weight);

void lato_context_resize_surface(LatoContext *lato_context, float x, float y,
                                 float width, float height);

LatoContext lato_context_init(float x, float y, float width, float height);

void lato_context_destroy(LatoContext *context);
