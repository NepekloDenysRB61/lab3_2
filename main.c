
#include <stdio.h>
#include <math.h>
#include <time.h>

// Функція варіанта 10: шукаємо корені рівняння f(x) = 0.
double f(double x)
{
    return 0.25 * pow(x - 25, 2) + pow(x + 25, 3) / 100 + 1;
}

// Перша похідна потрібна для перевірки єдиності кореня.
double d1(double x)
{
    return 0.5 * (x - 25) + 3 * pow(x + 25, 2) / 100;
}

// Друга похідна допомагає обрати нерухому точку хорди.
double d2(double x)
{
    return 0.5 + 6 * (x + 25) / 100;
}

// method = 1: половинне ділення; method = 2: метод хорд.
void solve(int method)
{
    // Межі ізоляції, точність, значення функції та похідної.
    double a, b, eps, fa, fb, da, db, vertex = -100.0 / 3.0;
    double lower, upper, t, x, fx, fixed = 0, fixed_f = 0;
    double old, min_d = 0, seconds = 0;
    double denominator, next_x, next_fx;
    // clock_t зберігає відлік таймера; секунди отримуємо після перетворення.
    clock_t start;

    // Ліміт, загальна кількість ітерацій та кількість у поточній групі.
    int limit, debug, iterations = 0, batch = 0;

    // Ознаки завершення і режиму обчислень.
    int unlimited = 0, found = 0, stopped = 0, stalled = 0, action;

    // Введення початкових умов і перевірка допустимості даних.
    printf("Input a and b: ");
    if (scanf("%lf%lf", &a, &b) != 2 || !isfinite(a) || !isfinite(b) || a >= b)
    {
        printf("Incorrect interval.\n");
        return;
    }

    printf("Input epsilon (> 0): ");
    if (scanf("%lf", &eps) != 1 || !isfinite(eps) || eps <= 0)
    {
        printf("Incorrect epsilon.\n");
        return;
    }

    printf("Maximum iterations: ");
    if (scanf("%d", &limit) != 1 || limit <= 0)
    {
        printf("Incorrect number of iterations.\n");
        return;
    }

    // Режим налагодження: 1 — друкувати кожну ітерацію, 0 — не друкувати.
    printf("Show each iteration? (1 - Yes, 0 - No): ");
    if (scanf("%d", &debug) != 1 || (debug != 0 && debug != 1))
    {
        printf("Incorrect debug mode.\n");
        return;
    }

    start = clock();
    fa = f(a);
    fb = f(b);

    // Скінченні аргументи також можуть спричинити переповнення f(x).
    if (!isfinite(fa) || !isfinite(fb))
    {
        printf("Function values are not finite. Choose another interval.\n");
        return;
    }

    // Точний корінь на межі повертаємо до перевірки похідної:
    // його значення вже відоме, навіть якщо єдиність не підтверджено.
    if (fa == 0 || fb == 0)
    {
        if (fa == 0) x = a;
        else x = b;

        printf("Root = %.10lf\n", x);
        printf("f(root) = 0\nIterations = 0\n");
        printf("CPU time = %.9lf seconds\n",
               (double)(clock() - start) / CLOCKS_PER_SEC);
        printf("Required accuracy achieved.\n");
        return;
    }

    // Для неперервної функції різні знаки на кінцях гарантують корінь.
    if ((fa > 0 && fb > 0) || (fa < 0 && fb < 0))
    {
        printf("No sign change on the interval.\n");
        return;
    }

    // Перевіряємо сталий знак першої похідної на всьому проміжку.
    // Для варіанта 10 d1(x) — парабола з вершиною у точці -100/3.
    da = d1(a);
    db = d1(b);
    lower = da;
    upper = da;
    if (db < lower) lower = db;
    if (db > upper) upper = db;

    if (a < vertex && vertex < b)
    {
        t = d1(vertex);
        if (t < lower) lower = t;
        if (t > upper) upper = t;
    }

    printf("\nf(a) = %.6lf, f(b) = %.6lf\n", fa, fb);
    printf("Range of f'(x): [%.6lf; %.6lf]\n", lower, upper);

    // Сталий ненульовий знак похідної дає строгу монотонність;
    // разом зі зміною знака f(x) це гарантує рівно один корінь.
    if (!(lower > 0 || upper < 0))
    {
        printf("Uniqueness of the root is not confirmed.\n");
        return;
    }
    printf("Exactly one root on the interval.\n");

    x = a;
    fx = fa;

    if (method == 2)
    {
        // Метод хорд потребує сталого ненульового знака другої похідної.
        if (!((d2(a) > 0 && d2(b) > 0) ||
              (d2(a) < 0 && d2(b) < 0)))
        {
            printf("Choose an interval with constant sign of f''(x).\n");
            return;
        }

        // Кінець, де f(x) і f''(x) мають однакові знаки, є нерухомим.
        if ((fa > 0 && d2(a) > 0) || (fa < 0 && d2(a) < 0))
        {
            fixed = a;
            fixed_f = fa;
            x = b;
            fx = fb;
        }
        else
        {
            fixed = b;
            fixed_f = fb;
        }

        // Мінімальний модуль похідної потрібен для оцінки похибки.
        min_d = fabs(da);
        if (fabs(db) < min_d) min_d = fabs(db);
        if (a < vertex && vertex < b && fabs(d1(vertex)) < min_d)
            min_d = fabs(d1(vertex));

        // Ділення на min_d допустиме лише для додатної скінченної оцінки.
        if (!isfinite(min_d) || min_d <= 0)
        {
            printf("Cannot estimate root accuracy.\n");
            return;
        }
    }

    // Накопичуємо час ділянок обчислення; очікування вибору не враховуємо.
    start = clock();

    while (!found && !stopped && !stalled)
    {
        // Після вичерпання ліміту надаємо три дії за умовою завдання.
        if (!unlimited && batch >= limit)
        {
            seconds += (double)(clock() - start) / CLOCKS_PER_SEC;
            printf("\nIteration limit reached (%d total).\n", iterations);

            do
            {
                printf("1 - Continue %d iterations\n", limit);
                printf("2 - Continue without limit\n");
                printf("3 - Stop and show result\n");
                printf("Choose: ");

                if (scanf("%d", &action) != 1)
                {
                    action = 3;
                    break;
                }
            } while (action < 1 || action > 3);

            start = clock();

            if (action == 3)
            {
                stopped = 1;
                break;
            }

            // Продовження на нову групу або без обмеження кількості.
            if (action == 1) batch = 0;
            else unlimited = 1;
        }

        if (method == 1)
        {
            // Половинне ділення: беремо середину поточного проміжку.
            x = a + (b - a) / 2;
            fx = f(x);
        }
        else
        {
            // Метод хорд: перетин хорди з віссю X дає нове наближення.
            denominator = fixed_f - fx;
            if (denominator == 0 || !isfinite(denominator))
            {
                stalled = 1;
                break;
            }

            // Перевіряємо нову точку до заміни останнього коректного наближення.
            next_x = x - fx * (fixed - x) / denominator;
            if (!isfinite(next_x) || next_x < a || next_x > b)
            {
                stalled = 1;
                break;
            }
            next_fx = f(next_x);
            if (!isfinite(next_fx))
            {
                stalled = 1;
                break;
            }

            old = x;
            x = next_x;
            fx = next_fx;
        }

        iterations++;
        batch++;

        if (debug == 1)
            printf("%d: x = %.10lf, f(x) = %.10lf\n", iterations, x, fx);

        if (method == 1)
        {
            // Похибка середньої точки не більша за половину довжини.
            if (fx == 0 || (b - a) / 2 <= eps)
                found = 1;
            else if (x == a || x == b)
                stalled = 1;
            else if ((fa < 0 && fx > 0) || (fa > 0 && fx < 0))
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
        else
        {
            // За теоремою Лагранжа |x - root| <= |f(x)| / min_d.
            // Мала зміна x сама по собі ще не гарантує потрібної точності.
            if (fx == 0 || fabs(fx) / min_d <= eps)
                found = 1;
            else if (x == old)
                stalled = 1;
        }
    }

    seconds += (double)(clock() - start) / CLOCKS_PER_SEC;

    // Спільний результат для обох методів.
    printf("\nRoot approximation = %.10lf\n", x);
    printf("f(root) = %.10lf\n", fx);
    printf("Iterations = %d\nCPU time = %.9lf seconds\n",
           iterations, seconds);

    if (found)
        printf("Required accuracy achieved.\n");
    else if (stalled)
        printf("Stopped: no numerical progress.\n");
    else
        printf("Stopped by user before required accuracy.\n");
}

// Головне меню викликає спільну функцію з потрібним методом.
int main(void)
{
    int choice;

    for (;;)
    {
        printf("\n1 - Bisection method\n");
        printf("2 - Chord method\n");
        printf("0 - Exit\nChoose: ");

        if (scanf("%d", &choice) != 1)
            return 0;

        if (choice == 0)
            return 0;

        if (choice == 1 || choice == 2)
            solve(choice);
        else
            printf("Incorrect option.\n");
    }
}
