#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <time.h>

double math_function(double x);
double derivative_1_math_funct(double x);
void research_function(void);
void bisection_method(void);

// Головна функція забезпечує вибір методу розв'язання рівняння.
int main()
{
    unsigned int var;

    for (;;)
    {
        // Виведення головного меню програми.
        printf("\n1 - Bisection method");
        printf("\n2 - Function research and tables");
        printf("\n0 - Exit");
        printf("\nChoose option: ");

        // Перевірка введеного пункту меню.
        if (scanf("%u", &var) != 1)
            return 0;

        switch (var)
        {
            // Пошук кореня методом половинного ділення.
            case 1:
                bisection_method();
                break;

            // Дослідження функції та пошук проміжків ізоляції.
            case 2:
                research_function();
                break;

            // Завершення роботи програми.
            case 0:
                return 0;

            default:
                printf("Incorrect option!\n");
                break;
        }
    }

    return 0;
}

// Основна математична функція для варіанта 10.
double math_function(double x)
{
    // pow підносить вираз до степеня: 2 — квадрат, 3 — куб.
    return 0.25 * pow(x - 25, 2) + pow(x + 25, 3) / 100 + 1;
}

// Обчислення першої похідної аналітичним методом.
double derivative_1_math_funct(double x)
{
    // Диференціюємо квадратний і кубічний доданки.
    return 0.5 * (x - 25) + 3 * pow(x + 25, 2) / 100;
}

// Метод половинного ділення для пошуку кореня рівняння.
void bisection_method(void)
{
    // a, b — межі проміжку, epsilon — допустима похибка.
    double a, b, epsilon;
    double x, fx, fa, fb;

    // Обмеження кількості ітерацій та їх фактична кількість.
    int max_iterations, iterations = 0;
    int found = 0;

    // Введення та перевірка меж проміжку.
    do
    {
        printf("\nInput a: ");
        if (scanf("%lf", &a) != 1)
            return;

        printf("Input b: ");
        if (scanf("%lf", &b) != 1)
            return;

    } while (a >= b);

    // Введення допустимої похибки.
    do
    {
        printf("Input epsilon (> 0): ");
        if (scanf("%lf", &epsilon) != 1)
            return;

    } while (epsilon <= 0);

    // Введення максимальної кількості ітерацій.
    do
    {
        printf("Input maximum iterations: ");
        if (scanf("%d", &max_iterations) != 1)
            return;

    } while (max_iterations <= 0);

    // Обчислення значень функції на межах проміжку.
    fa = math_function(a);
    fb = math_function(b);

    // Перевірка випадку, коли корінь знаходиться на межі.
    if (fa == 0 || fb == 0)
    {
        if (fa == 0)
            x = a;
        else
            x = b;

        printf("\nRoot = %.10lf", x);
        printf("\nf(root) = %.10lf", math_function(x));
        printf("\nIterations = 0\n");
        return;
    }

    // Перевірка різних знаків функції на кінцях проміжку.
    if ((fa > 0 && fb > 0) || (fa < 0 && fb < 0))
    {
        printf("\nFunction values have the same sign!\n");
        printf("Choose another interval.\n");
        return;
    }

    // Послідовне зменшення проміжку вдвічі.
    while (iterations < max_iterations)
    {
        // Знаходимо середину поточного проміжку.
        x = a + (b - a) / 2.0;
        fx = math_function(x);

        iterations++;

        // Перевіряємо досягнення заданої точності.
        if (fx == 0 || (b - a) / 2.0 <= epsilon)
        {
            found = 1;
            break;
        }

        // Залишаємо половину, в якій функція змінює знак.
        if ((fa < 0 && fx > 0) || (fa > 0 && fx < 0))
        {
            b = x;
            fb = fx;
        }
        else
        {
            a = x;
            fa = fx;
        }
    }

    // Виведення знайденого або проміжного результату.
    printf("\nRoot approximation = %.10lf", x);
    printf("\nf(root) = %.10lf", fx);
    printf("\nIterations = %d\n", iterations);

    if (found == 1)
        printf("Required accuracy achieved.\n");
    else
        printf("Maximum iterations reached. Accuracy not achieved.\n");
}

// Дослідження функції та побудова таблиць.
void research_function(void)
{
    // mode — спосіб введення, N — кількість точок, i — індекс, found — лічильник знахідок.
    unsigned int mode, N, i, found = 0;

    // isolated показує, чи підтверджено єдиність кореня на поточному інтервалі.
    int isolated;

    // X1, X2 — межі; delta — крок; x, fx — поточні дані; prev_x, prev_f — попередні.
    double X1, X2, delta, x, fx, prev_x, prev_f;

    // d1, d2 — перша похідна на кінцях інтервалу; vertex — аргумент її мінімуму.
    double d1, d2;
    double vertex = -100.0 / 3.0;

    // Вибір способу введення.
    do
    {
        printf("\n1 - Input X1, X2, N");
        printf("\n2 - Input X1, X2, delta");
        printf("\nChoose input method: ");

        if (scanf("%u", &mode) != 1)
            return;

    } while (mode != 1 && mode != 2);

    // Введення та перевірка меж.
    do
    {
        printf("Input X1: ");
        if (scanf("%lf", &X1) != 1)
            return;

        printf("Input X2: ");
        if (scanf("%lf", &X2) != 1)
            return;

    } while (X1 >= X2);

    if (mode == 1)
    {
        // Введення кількості точок.
        do
        {
            printf("Input N (2...1000): ");
            if (scanf("%u", &N) != 1)
                return;

        } while (N < 2 || N > 1000);

        // N точок утворюють N-1 проміжків.
        delta = (X2 - X1) / (N - 1);
    }
    else
    {
        // Введення кроку аргументу.
        do
        {
            printf("Input delta: ");
            if (scanf("%lf", &delta) != 1)
                return;

        } while (delta <= 0 ||
                 delta > X2 - X1 ||
                 (X2 - X1) / delta > 998);

        // Визначаємо кількість точок за кроком.
        N = (unsigned int)((X2 - X1) / delta) + 1;

        // Додаємо точку X2, якщо вона не потрапила до сітки.
        if (X1 + (N - 1) * delta < X2)
            N++;
    }

    // Виведення початкових даних.
    printf("\nX1 = %.6lf", X1);
    printf("\nX2 = %.6lf", X2);
    printf("\nN = %u", N);
    printf("\ndelta = %.6lf\n", delta);

    // Таблиця значень функції.
    printf("\nTable of function:\n");
    printf("-------------------------------------\n");
    printf(" N       x              f(x)\n");
    printf("-------------------------------------\n");

    // Перебираємо всі точки сітки.
    for (i = 0; i < N; i++)
    {
        x = X1 + i * delta;

        if (i == N - 1)
            x = X2;

        printf("%3u   %12.6lf   %12.6lf\n",
               i + 1, x, math_function(x));
    }

    // Таблиця значень першої похідної.
    printf("\nTable of derivative:\n");
    printf("-------------------------------------\n");
    printf(" N       x              f'(x)\n");
    printf("-------------------------------------\n");

    for (i = 0; i < N; i++)
    {
        x = X1 + i * delta;

        if (i == N - 1)
            x = X2;

        printf("%3u   %12.6lf   %12.6lf\n",
               i + 1, x, derivative_1_math_funct(x));
    }

    // Пошук проміжків зміни знака функції.
    printf("\nRoot isolation intervals:\n");

    prev_x = X1;
    prev_f = math_function(prev_x);

    // Порівнюємо значення у сусідніх точках.
    for (i = 1; i < N; i++)
    {
        x = X1 + i * delta;

        if (i == N - 1)
            x = X2;

        fx = math_function(x);

        if (prev_f == 0)
        {
            printf("Root at x = %.6lf\n", prev_x);
            found++;
        }

        // Зміна знака вказує на корінь між точками.
        if ((prev_f < 0 && fx > 0) ||
            (prev_f > 0 && fx < 0))
        {
            // Перевірка сталості знака похідної.
            d1 = derivative_1_math_funct(prev_x);
            d2 = derivative_1_math_funct(x);

            isolated = 0;

            if (d1 < 0 && d2 < 0)
                isolated = 1;

            // Для додатної похідної враховуємо її мінімум.
            if (d1 > 0 && d2 > 0 &&
                (vertex <= prev_x || vertex >= x ||
                 derivative_1_math_funct(vertex) > 0))
                isolated = 1;

            if (isolated == 1)
                printf("One root in [%.6lf; %.6lf]\n",
                       prev_x, x);
            else
                printf("Check interval [%.6lf; %.6lf]\n",
                       prev_x, x);

            found++;
        }

        // Поточна точка стає попередньою.
        prev_x = x;
        prev_f = fx;
    }

    if (prev_f == 0)
    {
        printf("Root at x = %.6lf\n", prev_x);
        found++;
    }

    if (found == 0)
        printf("No root intervals detected.\n");
}
