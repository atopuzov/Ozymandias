#ifndef CITY_CONSTANTS_H
#define CITY_CONSTANTS_H

enum {
    ADVISOR_NONE = 0,
    ADVISOR_LABOR = 1,
    ADVISOR_MILITARY = 2,
    ADVISOR_IMPERIAL = 3,
    ADVISOR_RATINGS = 4,
    ADVISOR_TRADE = 5,
    ADVISOR_POPULATION = 6,
    ADVISOR_HEALTH = 7,
    ADVISOR_EDUCATION = 8,
    ADVISOR_ENTERTAINMENT = 9,
    ADVISOR_RELIGION = 10,
    ADVISOR_FINANCIAL = 11,
    ADVISOR_CHIEF = 12,
    ADVISOR_HOUSING = 19
};

enum {
    int_NONE = 0,
    int_NO_FOOD = 1,
    int_NO_JOBS = 2,
    int_HIGH_TAXES = 3,
    int_LOW_WAGES = 4,
    int_MANY_TENTS = 5,
};

enum {
    NO_IMMIGRATION_LOW_WAGES = 0,
    NO_IMMIGRATION_NO_JOBS = 1,
    NO_IMMIGRATION_NO_FOOD = 2,
    NO_IMMIGRATION_HIGH_TAXES = 3,
    NO_IMMIGRATION_MANY_TENTS = 4,
    NO_IMMIGRATION_LOW_MOOD = 5
};

enum {
    FESTIVAL_NONE = 0,
    FESTIVAL_SMALL = 1,
    FESTIVAL_LARGE = 2,
    FESTIVAL_GRAND = 3
};

enum {
    GOD_CERES = 0,      GOD_OSIRIS = 0,
    GOD_NEPTUNE = 1,    GOD_RA = 1,
    GOD_MERCURY = 2,    GOD_PTAH = 2,
    GOD_MARS = 3,       GOD_SETH = 3,
    GOD_VENUS = 4,       GOD_BAST = 4
};

enum {
    TRADE_STATUS_NONE = 0,
    TRADE_STATUS_IMPORT = 1,
    TRADE_STATUS_EXPORT = 2,
    TRADE_STATUS_IMPORT_AS_NEEDED = 3,
};

#endif // CITY_CONSTANTS_H
