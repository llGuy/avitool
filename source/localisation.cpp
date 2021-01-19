#include <stdint.h>
#include "localisation.hpp"

struct language_strings_t {
    const char *strings[(uint32_t)locale_string_t::INVALID];
};

static language_t selected;
static language_strings_t languages[(uint32_t)language_t::INVALID];

void init_languages() {
    selected = language_t::FRENCH;

    language_strings_t *english = &languages[(uint32_t)language_t::ENGLISH];
    english->strings[(uint32_t)locale_string_t::BUTTON_FILE] = "File";
    english->strings[(uint32_t)locale_string_t::BUTTON_OPEN] = "Open";
    english->strings[(uint32_t)locale_string_t::BUTTON_SAVE] = "Save";
    english->strings[(uint32_t)locale_string_t::BUTTON_TOOLS] = "Tools";
    english->strings[(uint32_t)locale_string_t::BUTTON_BEGIN_RECORD] = "Begin record";
    english->strings[(uint32_t)locale_string_t::BUTTON_END_RECORD] = "End record";
    english->strings[(uint32_t)locale_string_t::BUTTON_MAKE_AXES] = "Make axes";
    english->strings[(uint32_t)locale_string_t::BUTTON_CHOOSE_RECORD_INFORMATION] = "Choose record information";
    english->strings[(uint32_t)locale_string_t::BUTTON_CANCEL] = "Cancel";
    english->strings[(uint32_t)locale_string_t::BUTTON_CLOSE] = "Close";
    english->strings[(uint32_t)locale_string_t::BUTTON_OK] = "OK";
    english->strings[(uint32_t)locale_string_t::TITLE_AXES] = "Axes";
    english->strings[(uint32_t)locale_string_t::TITLE_CHOOSE_RECORD_INFORMATION] = "Choose record information";
    english->strings[(uint32_t)locale_string_t::TITLE_CONTROLLER] = "Controller";
    english->strings[(uint32_t)locale_string_t::TITLE_OUTPUT] = "Output";
    english->strings[(uint32_t)locale_string_t::CHECKBOX_X_COORDINATE] = "X Coordinate";
    english->strings[(uint32_t)locale_string_t::CHECKBOX_Y_COORDINATE] = "Y Coordinate";
    english->strings[(uint32_t)locale_string_t::CHECKBOX_TIME] = "Time";
    english->strings[(uint32_t)locale_string_t::CHECKBOX_FRAME_ID] = "Frame ID";
    english->strings[(uint32_t)locale_string_t::INPUT_DISTANCE] = "Distance";
    english->strings[(uint32_t)locale_string_t::INPUT_COMMAND] = "Command";
    english->strings[(uint32_t)locale_string_t::COMMAND_LOAD_FILE] = "load_file";
    english->strings[(uint32_t)locale_string_t::COMMAND_GOTO_VIDEO_FRAME] = "goto_video_frame";
    english->strings[(uint32_t)locale_string_t::COMMAND_GOTO_VIDEO_TIME] = "goto_video_time";
    english->strings[(uint32_t)locale_string_t::COMMAND_BEGIN_RECORD] = "begin_record";
    english->strings[(uint32_t)locale_string_t::COMMAND_ADD_RECORD_POINT] = "add_record_point";
    english->strings[(uint32_t)locale_string_t::COMMAND_END_RECORD] = "end_record";
    english->strings[(uint32_t)locale_string_t::COMMAND_MAKE_AXES] = "make_axes";
    english->strings[(uint32_t)locale_string_t::TEXT_FAILED_TO_LOAD_FROM] = "Failed to load from";
    english->strings[(uint32_t)locale_string_t::TEXT_LOADED_FROM] = "Loaded from";
    english->strings[(uint32_t)locale_string_t::TEXT_RESOLUTION] = "Resolution";
    english->strings[(uint32_t)locale_string_t::TEXT_FRAME_COUNT] = "Frame count";
    english->strings[(uint32_t)locale_string_t::TEXT_LENGTH] = "Length";
    english->strings[(uint32_t)locale_string_t::TEXT_FPS] = "FPS";
    english->strings[(uint32_t)locale_string_t::TEXT_BEGAN_RECORDING] = "Began recording";
    english->strings[(uint32_t)locale_string_t::TEXT_FINISHED_RECORDING] = "Finished recording";
    english->strings[(uint32_t)locale_string_t::TEXT_FRAME] = "Frame";

    language_strings_t *french = &languages[(uint32_t)language_t::FRENCH];
    french->strings[(uint32_t)locale_string_t::BUTTON_FILE] = "Fichier";
    french->strings[(uint32_t)locale_string_t::BUTTON_OPEN] = "Ouvrir";
    french->strings[(uint32_t)locale_string_t::BUTTON_SAVE] = "Sauvegarder";
    french->strings[(uint32_t)locale_string_t::BUTTON_TOOLS] = "Outils";
    french->strings[(uint32_t)locale_string_t::BUTTON_BEGIN_RECORD] = "Commencer pointage";
    french->strings[(uint32_t)locale_string_t::BUTTON_END_RECORD] = "Finir pointage";
    french->strings[(uint32_t)locale_string_t::BUTTON_MAKE_AXES] = "Construire axes";
    french->strings[(uint32_t)locale_string_t::BUTTON_CHOOSE_RECORD_INFORMATION] = "Choisir l'information du pointage";
    french->strings[(uint32_t)locale_string_t::BUTTON_CANCEL] = "Annuler";
    french->strings[(uint32_t)locale_string_t::BUTTON_CLOSE] = "Fermer";
    french->strings[(uint32_t)locale_string_t::BUTTON_OK] = "OK";
    french->strings[(uint32_t)locale_string_t::TITLE_AXES] = "Axes";
    french->strings[(uint32_t)locale_string_t::TITLE_CHOOSE_RECORD_INFORMATION] = "Choisir l'information du pointage";
    french->strings[(uint32_t)locale_string_t::TITLE_CONTROLLER] = "Controleur";
    french->strings[(uint32_t)locale_string_t::TITLE_OUTPUT] = "Sortie";
    french->strings[(uint32_t)locale_string_t::CHECKBOX_X_COORDINATE] = "X";
    french->strings[(uint32_t)locale_string_t::CHECKBOX_Y_COORDINATE] = "Y";
    french->strings[(uint32_t)locale_string_t::CHECKBOX_TIME] = "Temps";
    french->strings[(uint32_t)locale_string_t::CHECKBOX_FRAME_ID] = "Numero de l'image";
    french->strings[(uint32_t)locale_string_t::INPUT_DISTANCE] = "Distance";
    french->strings[(uint32_t)locale_string_t::INPUT_COMMAND] = "Commande";
    french->strings[(uint32_t)locale_string_t::COMMAND_LOAD_FILE] = "load_file";
    french->strings[(uint32_t)locale_string_t::COMMAND_GOTO_VIDEO_FRAME] = "goto_video_frame";
    french->strings[(uint32_t)locale_string_t::COMMAND_GOTO_VIDEO_TIME] = "goto_video_time";
    french->strings[(uint32_t)locale_string_t::COMMAND_BEGIN_RECORD] = "begin_record";
    french->strings[(uint32_t)locale_string_t::COMMAND_ADD_RECORD_POINT] = "add_record_point";
    french->strings[(uint32_t)locale_string_t::COMMAND_END_RECORD] = "end_record";
    french->strings[(uint32_t)locale_string_t::COMMAND_MAKE_AXES] = "make_axes";
    french->strings[(uint32_t)locale_string_t::TEXT_FAILED_TO_LOAD_FROM] = "Erreur pendant l'ouverture du fichier";
    french->strings[(uint32_t)locale_string_t::TEXT_LOADED_FROM] = "Ouvert";
    french->strings[(uint32_t)locale_string_t::TEXT_RESOLUTION] = "Resolution";
    french->strings[(uint32_t)locale_string_t::TEXT_FRAME_COUNT] = "Nombre d'images";
    french->strings[(uint32_t)locale_string_t::TEXT_LENGTH] = "Duree";
    french->strings[(uint32_t)locale_string_t::TEXT_FPS] = "FPS";
    french->strings[(uint32_t)locale_string_t::TEXT_BEGAN_RECORDING] = "Pointage commence";
    french->strings[(uint32_t)locale_string_t::TEXT_FINISHED_RECORDING] = "Pointage finit";
    french->strings[(uint32_t)locale_string_t::TEXT_FRAME] = "Image";
}

void choose_language(language_t language) {
    selected = language;
}

const char *get_locale_string(locale_string_t s) {
    return languages[(uint32_t)selected].strings[(uint32_t)s];
}
