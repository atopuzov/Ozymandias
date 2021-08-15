#include "image.h"

#include "SDL.h"

//#include "core/buffer.h"
#include "core/file.h"
#include "core/io.h"
#include "core/log.h"
#include "core/mods.h"
#include "core/config.h"
#include "core/game_environment.h"
#include "core/image_collection.h"

#include <stdlib.h>
#include <string.h>

#define ENTRY_SIZE 64
#define NAME_SIZE 32

#define SCRATCH_DATA_SIZE 20000000

enum {
    NO_EXTRA_FONT = 0,
    FULL_CHARSET_IN_FONT = 1,
    MULTIBYTE_IN_FONT = 2
};

typedef struct font_files_collection {
    const char CYRILLIC_FONTS_555[NAME_SIZE];
    const char CYRILLIC_FONTS_SG2[NAME_SIZE];
    const char TRAD_CHINESE_FONTS_555[NAME_SIZE];
    const char KOREAN_FONTS_555[NAME_SIZE];
} font_files_collection;
font_files_collection ffcs[] = {
        {
                "C3_fonts.sg2",
                "C3_fonts.555",
                "rome.555",
                "korean.555",
        },
        {
                "",
                "",
                "",
                ""
        }
};

struct graphics_files_collection {
    const char C3_MAIN_555[3][NAME_SIZE];
    const char C3_MAIN_SG2[3][NAME_SIZE];
    const char C3_EDITOR_555[3][NAME_SIZE];
    const char C3_EDITOR_SG2[3][NAME_SIZE];
    const char C3_EMPIRE_555[NAME_SIZE];
    const char C3_ENEMY_555[20][NAME_SIZE];
    const char C3_ENEMY_SG2[20][NAME_SIZE];
    const char PH_MAIN_555[NAME_SIZE];
    const char PH_MAIN_SG3[NAME_SIZE];
    const char PH_UNLOADED_555[NAME_SIZE];
    const char PH_UNLOADED_SG3[NAME_SIZE];
    const char PH_TERRAIN_555[NAME_SIZE];
    const char PH_TERRAIN_SG3[NAME_SIZE];
    const char PH_FONTS_555[NAME_SIZE];
    const char PH_FONTS_SG3[NAME_SIZE];
    const char PH_EDITOR_GRAPHICS_555[NAME_SIZE];
    const char PH_EDITOR_GRAPHICS_SG3[NAME_SIZE];
    const char PH_EMPIRE_555[NAME_SIZE];
    const char PH_EMPIRE_SG3[NAME_SIZE];
    const char PH_ENEMY_555[20][NAME_SIZE];
    const char PH_ENEMY_SG2[20][NAME_SIZE];
    const char PH_EXPANSION_555[NAME_SIZE];
    const char PH_EXPANSION_SG3[NAME_SIZE];
    const char PH_SPRMAIN_555[NAME_SIZE];
    const char PH_SPRMAIN_SG3[NAME_SIZE];
    const char PH_SPRMAIN2_555[NAME_SIZE];
    const char PH_SPRMAIN2_SG3[NAME_SIZE];
    const char PH_SPRAMBIENT_555[NAME_SIZE];
    const char PH_SPRAMBIENT_SG3[NAME_SIZE];

} gfc = {
        {
                "c3.555",
                "c3_north.555",
                "c3_south.555"
        },
        {
                "c3.sg2",
                "c3_north.sg2",
                "c3_south.sg2"
        },
        {
                "c3map.555",
                "c3map_north.555",
                "c3map_south.555"
        },
        {
                "c3map.sg2",
                "c3map_north.sg2",
                "c3map_south.sg2"
        },
        "The_empire.555",
        {
                "goths.555",
                "Etruscan.555",
                "Etruscan.555",
                "carthage.555",
                "Greek.555",
                "Greek.555",
                "egyptians.555",
                "Persians.555",
                "Phoenician.555",
                "celts.555",
                "celts.555",
                "celts.555",
                "Gaul.555",
                "Gaul.555",
                "goths.555",
                "goths.555",
                "goths.555",
                "Phoenician.555",
                "North African.555",
                "Phoenician.555",
        },
        {
                "goths.sg2",
                "Etruscan.sg2",
                "Etruscan.sg2",
                "carthage.sg2",
                "Greek.sg2",
                "Greek.sg2",
                "egyptians.sg2",
                "Persians.sg2",
                "Phoenician.sg2",
                "celts.sg2",
                "celts.sg2",
                "celts.sg2",
                "Gaul.sg2",
                "Gaul.sg2",
                "goths.sg2",
                "goths.sg2",
                "goths.sg2",
                "Phoenician.sg2",
                "North African.sg2",
                "Phoenician.sg2",
        },
        "data/Pharaoh_General.555",
        "data/Pharaoh_General.sg3",
        "data/Pharaoh_Unloaded.555",
        "data/Pharaoh_Unloaded.sg3",
        "data/Pharaoh_Terrain.555",
        "data/Pharaoh_Terrain.sg3",
        "data/Pharaoh_Fonts.555",
        "data/Pharaoh_Fonts.sg3",
        "",
        "",
        "data/Empire.555",
        "data/Empire.sg3",
        {
                "data/Assyrian.555",
                "data/Egyptian.555",
                "data/Canaanite.555",
                "data/Enemy_1.555",
                "data/Hittite.555",
                "data/Hyksos.555",
                "data/Kushite.555",
                "data/Libian.555",
                "data/Mitani.555",
                "data/Nubian.555",
                "data/Persian.555",
                "data/Phoenician.555",
                "data/Roman.555",
                "data/SeaPeople.555"
        },
        {
                "data/Assyrian.sg3",
                "data/Egyptian.sg3",
                "data/Canaanite.sg3",
                "data/Enemy_1.sg3",
                "data/Hittite.sg3",
                "data/Hyksos.sg3",
                "data/Kushite.sg3",
                "data/Libian.sg3",
                "data/Mitani.sg3",
                "data/Nubian.sg3",
                "data/Persian.sg3",
                "data/Phoenician.sg3",
                "data/Roman.sg3",
                "data/SeaPeople.sg3"
        },
        "data/Expansion.555",
        "data/Expansion.sg3",
        "data/SprMain.555",
        "data/SprMain.sg3",
        "data/SprMain2.555",
        "data/SprMain2.sg3",
        "data/SprAmbient.555",
        "data/SprAmbient.sg3",
};

static struct {
    int current_climate;
    int is_editor;
    int fonts_enabled;
    int font_base_offset;

    image_collection *ph_expansion;
    image_collection *ph_sprmain;
    image_collection *ph_unloaded;
    image_collection *main;
    image_collection *ph_terrain;

    image_collection *ph_sprmain2;
    image_collection *ph_sprambient;
    image_collection *ph_mastaba;

    image_collection *enemy;
    image_collection *empire;
    image_collection *font;

    color_t *tmp_image_data;
} data = {
        -1,
        0,
        0,
        0,

        new image_collection,
        new image_collection,
        new image_collection,
        new image_collection,
        new image_collection,
        new image_collection,
        new image_collection,
        new image_collection,
        new image_collection,
        new image_collection,
        new image_collection,

        new color_t[SCRATCH_DATA_SIZE - 4000000]
};

int terrain_ph_offset = 0;

color_t to_32_bit(uint16_t c) {
    return ALPHA_OPAQUE |
           ((c & 0x7c00) << 9) | ((c & 0x7000) << 4) |
           ((c & 0x3e0) << 6) | ((c & 0x380) << 1) |
           ((c & 0x1f) << 3) | ((c & 0x1c) >> 2);
}

int convert_uncompressed(buffer *buf, int amount, color_t *dst) {
    for (int i = 0; i < amount; i += 2) {
        color_t c = to_32_bit(buf->read_u16());
        *dst = c;
        dst++;
    }
    return amount / 2;
}
int convert_compressed(buffer *buf, int amount, color_t *dst) {
    int dst_length = 0;
    while (amount > 0) {
        int control = buf->read_u8();
        if (control == 255) {
            // next byte = transparent pixels to skip
            *dst++ = 255;
            *dst++ = buf->read_u8();
            dst_length += 2;
            amount -= 2;
        } else {
            // control = number of concrete pixels
            *dst++ = control;
            for (int i = 0; i < control; i++) {
                *dst++ = to_32_bit(buf->read_u16());
            }
            dst_length += control + 1;
            amount -= control * 2 + 1;
        }
    }
    return dst_length;
}
static const color_t *load_external_data(const image *img) {
    char filename[FILE_NAME_MAX];
    int size = 0;
    buffer *buf = new buffer(img->draw.data_length);
    switch (GAME_ENV) {
        case ENGINE_ENV_C3:
            strcpy(&filename[0], "555/");
            strcpy(&filename[4], img->get_name());
            file_change_extension(filename, "555");
            size = io_read_file_part_into_buffer(
                    &filename[4], MAY_BE_LOCALIZED, buf,
                    img->draw.data_length, img->draw.offset - 1
            );
            break;
        case ENGINE_ENV_PHARAOH:
            strcpy(&filename[0], "Data/");
            strcpy(&filename[5], img->get_name());
            file_change_extension(filename, "555");
            size = io_read_file_part_into_buffer(
                    &filename[5], MAY_BE_LOCALIZED, buf,
                    img->draw.data_length, img->draw.offset - 1
            );
            break;
    }
    if (!size) {
        // try in 555 dir
        size = io_read_file_part_into_buffer(
                filename, MAY_BE_LOCALIZED, buf,
                img->draw.data_length, img->draw.offset - 1
        );
        if (!size) {
            log_error("unable to load external image", img->get_name(), 0);
            return nullptr;
        }
    }

    // NB: isometric images are never external
    if (img->draw.is_fully_compressed)
        convert_compressed(buf, img->draw.data_length, data.tmp_image_data);
    else
        convert_uncompressed(buf, img->draw.data_length, data.tmp_image_data);
    return data.tmp_image_data;
}

static image_collection *pak_from_collection_id(int collection) {
    switch (GAME_ENV) {
        case ENGINE_ENV_C3:
            return data.main; // only one for Caesar III
        case ENGINE_ENV_PHARAOH:
            switch (collection) {
                case IMAGE_COLLECTION_TERRAIN:
                    return data.ph_terrain;
                case IMAGE_COLLECTION_GENERAL:
                    return data.main;
                case IMAGE_COLLECTION_UNLOADED:
                    return data.ph_unloaded;
                case IMAGE_COLLECTION_EMPIRE:
                    return data.empire;
                case IMAGE_COLLECTION_SPR_MAIN:
                    return data.ph_sprmain;
                case IMAGE_COLLECTION_SPR_AMBIENT:
                    return data.ph_sprambient;
                    /////
                case IMAGE_COLLECTION_ENEMY:
                    return data.enemy;
                case IMAGE_COLLECTION_FONT:
                    return data.font;
                    /////
                case IMAGE_COLLECTION_EXPANSION:
                    return data.ph_expansion;
                case IMAGE_COLLECTION_EXPANSION_SPR:
                    return data.ph_sprmain2;
                    /////
                case IMAGE_COLLECTION_MASTABA:
                    return data.ph_mastaba;
            }
            break;
    }
    return nullptr;
}
int image_id_from_group(int collection, int group) {
    image_collection *pak = pak_from_collection_id(collection);
    if (pak == nullptr)
        return -1;
    return pak->get_id(group);
}
const image *image_get(int id, int mode) {
    switch (GAME_ENV) {
        case ENGINE_ENV_C3:
            if (id >= data.main->get_entry_count() && id < data.main->get_entry_count() + MAX_MODDED_IMAGES)
//                return mods_get_image(id);
                return nullptr;
            else if (id >= 0)
                return data.main->get_image(id);
            else
                return nullptr;
        case ENGINE_ENV_PHARAOH: // todo: mods
            const image *img;
            img = data.ph_expansion->get_image(id);
            if (img != nullptr) return img;
            img = data.ph_sprmain->get_image(id);
            if (img != nullptr) return img;
            img = data.ph_unloaded->get_image(id);
            if (img != nullptr) return img;
            img = data.main->get_image(id);
            if (img != nullptr) return img;
            img = data.ph_terrain->get_image(id);
            if (img != nullptr) return img;
            img = data.font->get_image(id);
            if (img != nullptr) return img;
            img = data.ph_sprambient->get_image(id);
            if (img != nullptr) return img;
            img = data.empire->get_image(id);
            if (img != nullptr) return img;

            // default
            return data.ph_terrain->get_image(615, true);
    }
//    return image_get(image_id_from_group(GROUP_TERRAIN_BLACK));
    return nullptr;
}
const image *image_letter(int letter_id) {
    if (data.fonts_enabled == FULL_CHARSET_IN_FONT)
        return data.font->get_image(data.font_base_offset + letter_id);
    else if (data.fonts_enabled == MULTIBYTE_IN_FONT && letter_id >= IMAGE_FONT_MULTIBYTE_OFFSET)
        return data.font->get_image(data.font_base_offset + letter_id - IMAGE_FONT_MULTIBYTE_OFFSET);
    else if (letter_id < IMAGE_FONT_MULTIBYTE_OFFSET)
        return image_get(image_id_from_group(GROUP_FONT) + letter_id);
    else {
        return nullptr;
    }
}
const image *image_get_enemy(int id) {
    return data.enemy->get_image(id);
}
const color_t *image_data(int id) {
    const image *lookup = image_get(id);
    const image *img = image_get(id + lookup->offset_mirror);
    if (img->draw.is_external)
        return load_external_data(img);
    else
        return img->get_data();; // todo: mods
}
const color_t *image_data_letter(int letter_id) {
    return image_letter(letter_id)->get_data();
}
const color_t *image_data_enemy(int id) {
    const image *lookup = image_get(id);
    const image *img = image_get(id + lookup->offset_mirror);
    id += img->offset_mirror;
    if (img->draw.offset > 0)
        return img->get_data();
    return nullptr;
}

int image_load_main(int climate_id, int is_editor, int force_reload) {
//    image_pak_table_generate();

    if (climate_id == data.current_climate && is_editor == data.is_editor && !force_reload)
        return 1;

    const char *filename_555;
    const char *filename_sgx;
    switch (GAME_ENV) {
        case ENGINE_ENV_C3:
            filename_555 = is_editor ? gfc.C3_EDITOR_555[climate_id] : gfc.C3_MAIN_555[climate_id];
            filename_sgx = is_editor ? gfc.C3_EDITOR_SG2[climate_id] : gfc.C3_MAIN_SG2[climate_id];
            if (!data.main->load_files(filename_555, filename_sgx))
                return 0;
            data.current_climate = climate_id;
            break;
        case ENGINE_ENV_PHARAOH:
            filename_555 = is_editor ? gfc.PH_EDITOR_GRAPHICS_555 : gfc.PH_MAIN_555;
            filename_sgx = is_editor ? gfc.PH_EDITOR_GRAPHICS_SG3 : gfc.PH_MAIN_SG3;
            if (!data.ph_expansion->load_files(gfc.PH_EXPANSION_555, gfc.PH_EXPANSION_SG3, -200)) return 0;
            if (!data.ph_sprmain->load_files(gfc.PH_SPRMAIN_555, gfc.PH_SPRMAIN_SG3, 700)) return 0;
            if (!data.ph_unloaded->load_files(gfc.PH_UNLOADED_555, gfc.PH_UNLOADED_SG3, 11025)) return 0;
            if (!data.main->load_files(filename_555, filename_sgx, 11706)) return 0;
            // ???? 539-long gap?
            if (!data.ph_terrain->load_files(gfc.PH_TERRAIN_555, gfc.PH_TERRAIN_SG3, 14252)) return 0;
            // ???? 64-long gap?
            if (!data.ph_sprambient->load_files(gfc.PH_SPRAMBIENT_555, gfc.PH_SPRAMBIENT_SG3, 15766+64)) return 0;
            if (!data.font->load_files(gfc.PH_FONTS_555, gfc.PH_FONTS_SG3, 18764)) return 0;
            if (!data.empire->load_files(gfc.PH_EMPIRE_555, gfc.PH_EMPIRE_SG3, 18764+1541)) return 0;
            break;
    }

    data.is_editor = is_editor;
    return 1;
}
int image_load_enemy(int enemy_id) {
    const char *filename_555;
    const char *filename_sgx;
    switch (GAME_ENV) {
        case ENGINE_ENV_C3:
            filename_555 = gfc.C3_ENEMY_555[enemy_id];
            filename_sgx = gfc.C3_ENEMY_SG2[enemy_id];
            break;
        case ENGINE_ENV_PHARAOH:
            filename_555 = gfc.PH_ENEMY_555[enemy_id];
            filename_sgx = gfc.PH_ENEMY_SG2[enemy_id];
            break;
    }

    if (!data.enemy->load_files(filename_555, filename_sgx))
        return 0;
    return 1;
}
int image_load_fonts(encoding_type encoding) {
    if (encoding == ENCODING_CYRILLIC)
        return 0;
    else if (encoding == ENCODING_TRADITIONAL_CHINESE)
        return 0;
    else if (encoding == ENCODING_SIMPLIFIED_CHINESE)
        return 0;
    else if (encoding == ENCODING_KOREAN)
        return 0;
    else {
//        free(data.font);
//        free(data.font_data);
//        data.font = 0;
//        data.font_data = 0;
        data.fonts_enabled = NO_EXTRA_FONT;
        return 1;
    }
}

void image::set_data(color_t *image_data, size_t size) {
    draw.data.reserve(size);
    std::copy(image_data, image_data + size, std::back_inserter(draw.data));
}
const color_t *image::image::get_data() const {
    return draw.data.data();
}
const char *image::get_name() const {
    return draw.bitmap_name.c_str();
}
void image::set_name(const char *filename) {
    draw.bitmap_name = std::string(filename);
}
void image::set_name(const char *filename, size_t size) {
    draw.bitmap_name = std::string(filename, size);
}