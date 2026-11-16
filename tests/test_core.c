// Unit test cho phan logic nghiep vu (khong phu thuoc giao dien Windows).
// Build & chay:  cmake -S . -B build-test && cmake --build build-test && ctest --test-dir build-test
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "date.h"
#include "expense.h"
#include "invoice.h"

static int g_total = 0, g_failed = 0;

#define CHECK(cond, ...) do {                                         \
        g_total++;                                                    \
        if (!(cond)) {                                                \
            g_failed++;                                               \
            printf("  FAIL %s:%d: ", __FILE__, __LINE__);             \
            printf(__VA_ARGS__);                                      \
            printf("\n");                                             \
        }                                                             \
    } while (0)

#define CHECK_INT(actual, expected) \
    CHECK((actual) == (expected), "%s = %d, mong doi %d", #actual, (int)(actual), (int)(expected))

#define CHECK_NEAR(actual, expected) \
    CHECK(fabs((double)(actual) - (double)(expected)) < 0.01, \
          "%s = %.3f, mong doi %.3f", #actual, (double)(actual), (double)(expected))

#define CHECK_STR(actual, expected) \
    CHECK(strcmp((actual), (expected)) == 0, "\"%s\", mong doi \"%s\"", (actual), (expected))

// ---------------------------------------------------------------- date.c
static Date D(int d, int m, int y) { Date x = {d, m, y}; return x; }

static void test_date(void) {
    printf("[date]\n");
    Date a, b;

    CHECK_INT(parseDate("25/03/2024", &a), 1);
    CHECK_INT(a.day, 25); CHECK_INT(a.month, 3); CHECK_INT(a.year, 2024);
    CHECK_INT(parseDate("29/02/2024", &a), 1);   // nam nhuan
    CHECK_INT(parseDate("29/02/2023", &a), 0);   // khong nhuan
    CHECK_INT(parseDate("29/02/1900", &a), 0);   // chia het 100, khong nhuan
    CHECK_INT(parseDate("29/02/2000", &a), 1);   // chia het 400, nhuan
    CHECK_INT(parseDate("31/04/2024", &a), 0);
    CHECK_INT(parseDate("01/13/2024", &a), 0);
    CHECK_INT(parseDate("abc", &a), 0);

    a = D(25, 3, 2024); b = D(25, 4, 2024);
    CHECK_INT(daysBetween(&a, &b), 31);
    a = D(1, 2, 2024);  b = D(1, 3, 2024);
    CHECK_INT(daysBetween(&a, &b), 29);
    a = D(1, 2, 2023);  b = D(1, 3, 2023);
    CHECK_INT(daysBetween(&a, &b), 28);
    a = D(31, 12, 2023); b = D(1, 1, 2024);
    CHECK_INT(daysBetween(&a, &b), 1);
    a = D(1, 1, 2023); b = D(1, 1, 2024);
    CHECK_INT(daysBetween(&a, &b), 365);

    a = D(1, 1, 2024); b = D(2, 1, 2024);
    CHECK_INT(compareDate(&a, &b), -1);
    CHECK_INT(compareDate(&b, &a), 1);
    CHECK_INT(compareDate(&a, &a), 0);

    char buf[16];
    a = D(5, 7, 2024);
    dateToString(&a, buf);
    CHECK_STR(buf, "05/07/2024");
}

// ------------------------------------------------------------- expense.c
// Bang gia bac thang giong build/giadien.txt (don vi: nghin VND / kWh)
static struct tariff EVN[] = {
    {1,   0,   50,      1.984f},
    {2,  51,  100,      2.050f},
    {3, 101,  200,      2.380f},
    {4, 201,  300,      2.998f},
    {5, 301,  400,      3.350f},
    {6, 401, 1000000,   3.460f},
};
#define N_EVN ((int)(sizeof(EVN) / sizeof(EVN[0])))

static void test_calc_amount(void) {
    printf("[calc_amount]\n");
    CHECK_NEAR(calc_amount(0,   EVN, N_EVN), 0.0);
    CHECK_NEAR(calc_amount(1,   EVN, N_EVN), 1.984);
    // Bac 1 phai chua dung 50 kWh, khong phai 51
    CHECK_NEAR(calc_amount(50,  EVN, N_EVN), 50 * 1.984);
    CHECK_NEAR(calc_amount(51,  EVN, N_EVN), 50 * 1.984 + 1 * 2.050);
    CHECK_NEAR(calc_amount(100, EVN, N_EVN), 50 * 1.984 + 50 * 2.050);
    // Vi du thuc te trong build/invoice.txt: KH000001, ky 4, 259 kWh
    CHECK_NEAR(calc_amount(259, EVN, N_EVN),
               50 * 1.984 + 50 * 2.050 + 100 * 2.380 + 59 * 2.998);
    CHECK_NEAR(calc_amount(500, EVN, N_EVN),
               50 * 1.984 + 50 * 2.050 + 100 * 2.380 + 100 * 2.998
               + 100 * 3.350 + 100 * 3.460);

    // Bang gia bat dau tu 1 (1-50, 51-100) cung phai cho ket qua giong het
    struct tariff from1[] = {{1, 1, 50, 1.0f}, {2, 51, 100, 2.0f}};
    CHECK_NEAR(calc_amount(50, from1, 2), 50.0);
    CHECK_NEAR(calc_amount(80, from1, 2), 50.0 + 30 * 2.0);
}

static void test_format_number(void) {
    printf("[format_number]\n");
    char s[64];
    format_number(s, 0.0f);        CHECK_STR(s, "0.00");
    format_number(s, 999.0f);      CHECK_STR(s, "999.00");
    format_number(s, 1000.0f);     CHECK_STR(s, "1,000.00");
    format_number(s, 616582.0f);   CHECK_STR(s, "616,582.00");
    format_number(s, 1234567.5f);  CHECK_STR(s, "1,234,567.50");
}

// ------------------------------------------------------------- invoice.c
static void test_number_to_text(void) {
    printf("[number_to_text]\n");
    static const struct { int n; const char* text; } cases[] = {
        {0,          "Khong"},
        {5,          "Nam"},
        {10,         "Muoi"},
        {15,         "Muoi lam"},
        {21,         "Hai muoi mot"},
        {105,        "Mot tram le nam"},
        {677,        "Sau tram bay muoi bay"},
        {1005,       "Mot nghin khong tram le nam"},
        {5000,       "Nam nghin"},
        {15000,      "Muoi lam nghin"},
        {105000,     "Mot tram le nam nghin"},
        {120000,     "Mot tram hai muoi nghin"},
        {150000,     "Mot tram nam muoi nghin"},
        {677120,     "Sau tram bay muoi bay nghin mot tram hai muoi"},
        {1000000,    "Mot trieu"},
        {1500000,    "Mot trieu nam tram nghin"},
        {2000005,    "Hai trieu khong tram le nam"},
        {1234567890, "Mot ty hai tram ba muoi bon trieu nam tram sau muoi bay nghin tam tram chin muoi"},
    };
    char buf[256];
    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        memset(buf, 0, sizeof(buf));
        CHECK(number_to_text(cases[i].n, buf) == SUCCESS, "number_to_text(%d) tra ve loi", cases[i].n);
        CHECK(strcmp(buf, cases[i].text) == 0, "number_to_text(%d) = \"%s\", mong doi \"%s\"",
              cases[i].n, buf, cases[i].text);
    }
    CHECK(number_to_text(1, NULL) != SUCCESS, "NULL buffer phai bao loi");
}

int main(void) {
    test_date();
    test_calc_amount();
    test_format_number();
    test_number_to_text();
    printf("\n%d/%d kiem tra dat\n", g_total - g_failed, g_total);
    return g_failed ? 1 : 0;
}
