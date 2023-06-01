/** @file
 * Implementacja klasy przechowującej przekierowania numerów telefonicznych
 *
 * @authors Marcin Peczarski <marpe@mimuw.edu.pl>,
 *  Mateusz Mroczka <mm439937@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#include <stdbool.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include "phone_forward.h"

/**
 * Maksymalna ilość synów pojedyńczego węzła,
 * zależna od podstawy używanego systemu liczbowego.
 */
#define BASE 12

/** @brief To jest struktura przechowująca
 * przekierowania numerów telefonów.
 * Węzeł drzewa przechowującego cyfrę numeru telefonu, przekierowanie oraz
 * wskaźniki na synów danego węzła.
 * Numer odczytujemy jako numery kolejnych ojców danego węzła.
 */
struct PhoneForward {
    /** 
     * Napis reprezentujący napis telefonu, na który mamy przekierowanie. 
     */
    char *forwardNumber;
    /**
     * Tablica wskaźników na dzieci danego węzła. 
     */
    PhoneForward *children[BASE];
};

/** @brief To jest struktura przechowująca ciąg numerów telefonów.
 * Struktura przechowująca tablicę wskaźników na napisy,
 * będące numerami telefonów
 * oraz rozmiar tej tablicy.
 */
struct PhoneNumbers {
    /**
     * Tablica wskaźników na napisy, rerezentujące numery
     */
    char **numbers;
    /**
     * Rozmiar tablicy numerów wskaźników.
     */
    size_t size;
};

/** @brief Sprawdza popraność symbolu.
 * Sprawdza, czy podany symbol należy do
 * zadanego w zadaniu systemu liczbowego.
 * Akceptuje więc cyfry od 0 do 9 oraz znaki * i #.
 * @param[in] c – sprawdzany znak.
 * @return Wartość @p true, gdy znak jest cyfrą podanego systemu liczbowego.
 *         Wartość @p false, w przeciwnym przypadku.
 */
static bool isSymbol(char c) {
    return isdigit(c) || c == '*' || c == '#';
}

/** @brief Zmienia znak typu char na odpowiadającą mu liczbę typu short.
 * Znaki reprezentujące cyfry zmienia na odpowiadającą cyfrę,
 * znak '*' tratkuje jako 10 a znak '#' jako 11.
 * @param[in] c – zmieniany znak.
 * @return Wartość typu short, odpowiednią dla podanego znaku.
 */
static short charToInt(char c) {
    if (isdigit(c)) return c - '0';
    if (c == '*')   return 10;
    return 11;
}

/** @brief Zmienia podaną liczbę typu short na odpowiadający jej znak.
 * Odwrotność funkcji @ref charToInt;
 * @param[in] x – zmieniana liczba.
 * @return Wartość typu char, odpowiednią dla podanej liczby.
 */
static char intToChar(short x) {
    if (x >= 0 && x <= 9)   return '0' + x;
    if (x == 10)    return '*';
    return '#';
}

PhoneForward * phfwdNew(void) {
    PhoneForward *pf = malloc(sizeof(PhoneForward));
    if (pf == NULL) {
        return NULL;
    }
    for (short i = 0; i < BASE; i++) {
        pf->children[i] = NULL;
    }
    pf->forwardNumber = NULL;
    return pf;
}

void phfwdDelete(PhoneForward *pf) {
    if (pf != NULL) {
        for (short i = 0; i < BASE; i++) {
            phfwdDelete(pf->children[i]);
        }
        if (pf->forwardNumber != NULL){
            free(pf->forwardNumber);
        }
        free(pf);
    }
}


/** @brief Sprawdza poprawność podanego numeru.
 * Sprawdza poprawność podanego numeru @p num.
 * Sprawdza, czy @p num nie jest NULL-em, nie jest pusty lub nie zawiera znaku,
 * niebędącego cyfrą.
 * @param[in] num – wskaźnik na sprawdzany napis.
 * @return Wartość @p true, jeśli napis jest poprawną liczbą.
 *         Wartość @p false, jeśli napis nie jest poprawną liczbą,
 *         jest pustym napisem lub wskazuje na NULL.
 */
static bool isNumberOk(char const *num) {
    if (num == NULL)    return false;
    if (strcmp(num, "") == 0)   return false;
    size_t i = 0;
    while (num[i] != '\0') {
        if (!isSymbol(num[i]))   return false;
        i++;
    }
    return true;
}

/** @brief Dodaje przekierowanie numeru telefonu.
 * Schodzi odpowiednio w drzewie przekierowań,
 * po czym dodaje odpowiednie węzły, aż do węzła reprezentującego
 * @ p num1, w którym zapisuje @ p num2.
 * @param[in] pf – wskaźnik na obecny węzeł drzewa przekierowań.
 * @param[in] num1 – wskaźnik na numer, będący prefiksem przekierowań.
 * @param[in] num2 – wskaźnik na numer, na który tworzymy przekierowanie.
 * @param[in] i – obecny indeks cyfry w @ p num1. 
 * @return Wartość @p true, jeśli dodawanie przekierowania się powiodło.
 *         Wartość @p false, w przeciwnym przypadku.
 */
static bool addPhoneForward(PhoneForward *pf,
        char const *num1, char const *num2, size_t i) {
    assert(pf != NULL);
    if (num1[i] == '\0') {
        if (pf->forwardNumber != NULL) {
            free(pf->forwardNumber);
        }
        pf->forwardNumber = malloc((size_t) (strlen(num2) + 1) * sizeof(char));
        strcpy(pf->forwardNumber, num2);
    }
    else {
        if (pf->children[charToInt(num1[i])] == NULL) {
            pf->children[charToInt(num1[i])] = phfwdNew();
            if (pf->children[charToInt(num1[i])] == NULL) return false;
        }
        return addPhoneForward(pf->children[charToInt(num1[i])], 
                num1, num2, i + 1);
    }
    return true;
}

bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2) {
    if (pf == NULL) {
        return false;
    }
    if (!isNumberOk(num1) || !isNumberOk(num2) || !strcmp(num1, num2)) {
        return false;
    }

    bool result = addPhoneForward(pf, num1, num2, 0);
    if (!result) {
        phfwdRemove(pf, num1);
    }
    return result;
}

/** @brief Rekurencyjnie usuwa przekierowania.
 * Usuwa przekierowania, których parametr @p num jest prefiksem.
 * @param[in] pf – wskaźnik na obecny węzeł. 
 * @param[in] num – wskaźnik na sprawdzany napis.
 * @param[in] lenght – długość napisu @p num.
 * @param[in] i – obecny indeks cyfry w @ p num.  
 */
static void phoneForwardRemove(PhoneForward *pf,
        char const *num, size_t lenght, size_t i) {
    if (pf != NULL) {
        if (i == lenght - 1) {
            phfwdDelete(pf->children[charToInt(num[i])]);
            pf->children[charToInt(num[i])] = NULL;
        }
        else {
            phoneForwardRemove(pf->children[charToInt(num[i])],
                    num, lenght, i + 1);
        }
    }
}

void phfwdRemove(PhoneForward *pf, char const *num) {
    if (pf == NULL || num == NULL || !isNumberOk(num)) {
        return;
    }
    size_t lenght = strlen(num);
    if (lenght == 0) {
        return;
    }
    phoneForwardRemove(pf, num, lenght, 0);
}

/** @brief Tworzy nową strukturę PhoneNumbers.
 * Alokuje pamięć na nową strukturę PhoneNumbers oraz
 * pamięć na pierwszą komórkę tablicy wskaźników na napisy.
 * Ustawia parametr @p size na 0.
 * @return wskaźnik na zaalokowaną pamięć.
 */
static PhoneNumbers * phnumNew(void) {
    PhoneNumbers *pnums;
    pnums = malloc(sizeof(PhoneNumbers));
    pnums->size = 0;
    pnums->numbers = (char **)malloc(sizeof(char *));
    pnums->numbers[0] = NULL;
    return pnums;
}


/** @brief Przechodzi przez drzewo PhoneForward po podanym numerze.
 * Przechodzi przez drzewo PhoneForward,
 * zapamiętując ostatnie nie będące NULL-em
 * przekierowanie oraz na indeksie @p j indeks, 
 * na którym znaleziono to przekierowanie.
 * @param[in] pf – wskaźnik na obecny węzeł PhoneForward.
 * @param[in] num – wskaźnik na numer, którego przekierowania szukamy.
 * @param[in] pn – wskaźnik na PhoneNumber,
 *                 zapamiętujący ostatnie napotkane przekierowanie.
 * @param[in] j – wskaźnik na indeks ostatniego napotkanego przekierowania.
 * @param[in] i – indeks na obecną cyfrę w numerze @p num.
 */
static void phoneForwardGet(PhoneForward const *pf, char const *num,
        char **pn, size_t *j, size_t i) {
    if (pf->forwardNumber != NULL) {
        (*pn) = pf->forwardNumber;
        (*j) = i;
    }
    if (num[i] != '\0' && pf->children[charToInt(num[i])] != NULL) {
        phoneForwardGet(pf->children[charToInt(num[i])], num, pn, j, i + 1);
    }
}

PhoneNumbers * phfwdGet(PhoneForward const *pf, char const *num) {
    if (pf == NULL) {
        return NULL;
    }
    PhoneNumbers *pnums = phnumNew(); 

    if (!isNumberOk(num)) {
        pnums->size = 1;
        return pnums;
    }

    char *pn = NULL;
    size_t j = 0;
    
    phoneForwardGet(pf, num, &pn, &j, 0);

    if (pn == NULL) {
        pnums->numbers[0] = malloc(sizeof(char));
        pnums->numbers[0][0] = '\0';
    }
    else {
        pnums->numbers[0] = malloc((size_t) (strlen(pn) + 1) * sizeof(char));
        strcpy(pnums->numbers[0], pn);
    }

    size_t len = strlen(pnums->numbers[0]);
    pnums->numbers[0] = realloc(pnums->numbers[0],
            (size_t) ((len + strlen(num) - j + 1) * sizeof(char)));
    strncpy(pnums->numbers[0] + len, num + j, strlen(num) - j + 1);

    pnums->size = 1;
    return pnums;
}

/**
 * @brief Sprawdza, czy numer jest prefiksem innego podanego numeru.
 * Sprawdza, czy napis zawiera się
 * w początkowym ciągu znaków innego napisu.
 * @param[in] prefix – napis, dla którego sprawdzamy, czy jest prefiksem.
 * @param[in] num – napis, dla którego sprawdzamy,
 *                  czy drugi napis jest jego prefiksem.
 * @return Wartość @p true, gdy napis jest prefiksem drugiego napisu.
 *         Wartość @p false, w przeciwnym przypadku.
 */
static bool isPrefixOf(char *prefix, char const *num) {
    size_t len = strlen(prefix);
    if (len > strlen(num)) {
        return false;
    }
    for (size_t i = 0; i < len; i++) {
        if (prefix[i] != num[i]) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Zwraca większy rozmiar tablic.
 * Zwraca 2 * @p size + 1,
 * jeśli nie przekroczy to maksymalnego rozmiaru size_t
 * lub maksymalny rozmiar size_t w przeciwnym przypadku.
 * @param[in] size – rozmiar, który chcemy odpowienio powiększyć.
 * @return Nowy, powiększony rozmiar.
 */
static size_t newSize(size_t size) {
    if (size > SIZE_MAX / 2 - 1) return SIZE_MAX;
    return size * 2 + 1;
}

/**
 * @brief Zwraca mniejszą z dwóch liczb.
 * @param[in] a – podana liczba.
 * @param[in] b – druga podana liczba.
 * @return Mniejsza z podanych liczb.
 */
static size_t min(size_t a, size_t b) {
    if (a <= b) return a;
    else    return b;
}

/**
 * @brief Zwraca mniejszy z dwóch napisów w porządku leksykograficznym.
 * @param[in] a – podany napis.
 * @param[in] b – drugi podany napis.
 * @return -1 jeśli napis a jest w porządku leksykograficznym przed b.
 *          0 jeśli napisy są sobie równe.
 *          1 jeśli napis b jest w porządku leksykograficznym przed a.
 */
static int comparator(const void *a, const void *b) {
    char *num1 = *(char * const *) a;
    char *num2 = *(char * const *) b;

    size_t len1 = strlen(num1);
    size_t len2 = strlen(num2);

    for (size_t i = 0; i < min(len1, len2); i++) {
        if (charToInt(num1[i]) > charToInt(num2[i]))    return 1;
        if (charToInt(num1[i]) < charToInt(num2[i]))    return -1;
    }

    if (len1 < len2)   return -1;
    if (len1 > len2)    return 1;
    return 0;
}

/**
 * @brief Znajduje numery,
 *  które powinny być zawarte w wyniku funkcji @ref phfwdReverse.
 * Przechodzi po drzewie @p pf znając ciąg ojców obecnego węzła,
 * sprawdza, czy obecne przekierowanie pozwala na dodanie
 * odpowiedniego numeru do struktury @p pn.
 * @param[in] pf – Wskaźnik na strukturę PhoneForward,
 *                 dla której wykonywana jest funkcja phfwdReverse.
 * @param[in] reverseNum – Wskaźnik na napis reprezentujący numer,
 *                         dla którego wykonywana jest funckja
 *                         @ref phfwdReverse.
 * @param[in, out] currentNum – Wskaźnik na wskaźnik na napis
 *                              reprezentujący numer, 
 *                              którego przekierowanie obecnie sprawdzamy.
 * @param[in] index – Liczba reprezentująca głębokość obecnego węzła.
 * @param[in] currentNumSize – Wskaźnik na napis
 *                             reprezentujący obecny rozmiar tablicy 
 *                             @p pn->Numbers.
 * @param[in, out] pn – Wskaźnik na strukturę PhoneNumbers,
 *                      do której dodajemy znalezione numery.
 * @param[in, out] j – Wskaźnik na liczbę
 *                     reprezentującą obecną ilość numerów w @p pn.
 */
static void reverse(PhoneForward const *pf, char const *reverseNum,
        char **currentNum, size_t index, size_t (*currentNumSize),
        PhoneNumbers *pn, size_t *j) {
    if (pf != NULL) {
        if (pf->forwardNumber != NULL 
                && isPrefixOf(pf->forwardNumber, reverseNum)) {

            if ((*j) == pn->size) {
                pn->size = newSize(pn->size);
                pn->numbers = realloc(pn->numbers, pn->size * sizeof(char *));
            }
            
            size_t lenReverseNum = strlen(reverseNum);
            size_t lenForwardNumber = strlen(pf->forwardNumber);
            pn->numbers[(*j)] = malloc((
                        index + lenReverseNum - lenForwardNumber + 1)
                        * sizeof(char));
            strncpy(pn->numbers[(*j)], (*currentNum), index);
            strncpy(pn->numbers[(*j)] + index,
                    reverseNum + lenForwardNumber,
                    lenReverseNum - lenForwardNumber + 1);
            (*j)++;
        }
        
        for (short i = 0; i < BASE; i++) {
            if (index == (*currentNumSize)) {
                (*currentNumSize) = newSize((*currentNumSize));
                (*currentNum) = realloc((*currentNum),
                        (*currentNumSize) * sizeof(char));
            }
            (*currentNum)[index] = intToChar(i);
            reverse(pf->children[i], reverseNum, currentNum,
                    index + 1, currentNumSize, pn, j);
        }
    }
}

PhoneNumbers * phfwdReverse(PhoneForward const *pf,  char const *num) {
    if (pf == NULL) {
        return NULL;
    }
    PhoneNumbers *pn = phnumNew();
    if (!isNumberOk(num)) {
        pn->size = 1;
        return pn;
    }

    char *currentNum = NULL;
    size_t currentNumSize = 0;
    size_t j = 0;
    reverse(pf, num, &currentNum, 0, &currentNumSize, pn, &j);
    free(currentNum);
    
    pn->numbers = realloc(pn->numbers, (j + 1) * sizeof(char *));
    pn->size = j + 1;
    pn->numbers[j] = malloc((strlen(num) + 1) * sizeof(char));
    strcpy(pn->numbers[j], num);

    qsort((void *) pn->numbers, pn->size, sizeof(char *), comparator);

    PhoneNumbers *pnReturn = phnumNew();
    pnReturn->numbers = realloc(pnReturn->numbers, pn->size * sizeof(char *));
    size_t k = 0;
    for (size_t i = 0; i < pn->size - 1; i++) {
        if (strcmp(pn->numbers[i], pn->numbers[i + 1]) != 0) {
            pnReturn->numbers[k] = malloc((
                        strlen(pn->numbers[i]) + 1) * sizeof(char));
            strcpy(pnReturn->numbers[k], pn->numbers[i]);
            k++;
        }
    }

    pnReturn->numbers = realloc(pnReturn->numbers, (k + 1) * sizeof(char *));
    pnReturn->numbers[k] = malloc((
                strlen(pn->numbers[pn->size - 1]) + 1) * sizeof(char));
    strcpy(pnReturn->numbers[k], pn->numbers[pn->size - 1]);
    pnReturn->size = k + 1;

    phnumDelete(pn);
    return pnReturn;
}

PhoneNumbers * phfwdGetReverse(PhoneForward const *pf, char const *num) {
    if (pf == NULL) {
        return NULL;
    }
    PhoneNumbers *pnReturn = phnumNew();
    if (!isNumberOk(num)) {
        pnReturn->size = 1;
        return pnReturn;
    }
    PhoneNumbers *pn = phfwdReverse(pf, num);
    size_t k = 0;
    for (size_t i = 0; i < pn->size; i++) {
        PhoneNumbers * testPn = phfwdGet(pf, pn->numbers[i]);
        if (testPn != NULL && testPn->size != 0
                && !strcmp(testPn->numbers[0], num)) {
            if (k == pnReturn->size) {
                pnReturn->size = newSize(pnReturn->size);
                pnReturn->numbers = realloc(pnReturn->numbers,
                        pnReturn->size * sizeof(char *));
            }
            pnReturn->numbers[k] = malloc(
                    (strlen(pn->numbers[i]) + 1) * sizeof(char));
            strcpy(pnReturn->numbers[k], pn->numbers[i]);
            k++;
        }
       phnumDelete(testPn); 
    }

    pnReturn->size = k;
    pnReturn->numbers = realloc(pnReturn->numbers, k * sizeof(char *));
    phnumDelete(pn);
    return pnReturn;
}

void phnumDelete(PhoneNumbers *pnum) {
    if (pnum != NULL) {
        for(size_t i = 0; i < pnum->size; i++) {
            free(pnum->numbers[i]);
        }

        free(pnum->numbers);
        free(pnum);
    }
}

char const * phnumGet(PhoneNumbers const *pnum, size_t idx) {
    if (pnum == NULL) {
        return NULL;
    }
    if (idx >= pnum->size)  return NULL;
    return pnum->numbers[idx];
}

