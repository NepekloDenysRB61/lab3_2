/**
 * @file main.c
 * @brief Консольна програма розв'язання нелінійного рівняння для варіанта 10.
 * @details Реалізує метод половинного ділення та метод хорд із нерухомим
 *          кінцем. Перед пошуком перевіряє допустимість даних, існування
 *          кореня за зміною знака функції та його єдиність за знаком похідної.
 *          Підтримує виведення ітерацій, продовження після досягнення ліміту
 *          та вимірювання часу обчислювальних ділянок без очікування введення.
 */

#include <stdio.h>

#include <math.h>

#include <time.h>

/**
 * @brief Обчислює значення математичної функції варіанта 10.
 * @param[in] x Аргумент функції.
 * @return Значення f(x) = 0.25*(x - 25)^2 + (x + 25)^3/100 + 1.
 * @note Поліном неперервний на всій числовій осі. За надто великого
 *       модуля аргументу обчислення у типі double може переповнитися.
 */
double f(double x)
{
    return 0.25 * pow(x - 25, 2) + pow(x + 25, 3) / 100 + 1;
}

/**
 * @brief Обчислює першу похідну математичної функції.
 * @param[in] x Точка обчислення похідної.
 * @return Значення f'(x) = 0.5*(x - 25) + 3*(x + 25)^2/100.
 * @details Використовується для перевірки строгої монотонності функції
 *          та оцінювання абсолютної похибки наближення методом хорд.
 */
double d1(double x)
{
    return 0.5 * (x - 25) + 3 * pow(x + 25, 2) / 100;
}

/**
 * @brief Обчислює другу похідну математичної функції.
 * @param[in] x Точка обчислення похідної.
 * @return Значення f''(x) = 0.5 + 6*(x + 25)/100.
 * @details Визначає сталість опуклості та вибір нерухомого кінця хорди
 *          за умовою однакового знака f(x) і f''(x).
 */
double d2(double x)
{
    return 0.5 + 6 * (x + 25) / 100;
}

/**
 * @brief Виконує пошук кореня вибраним чисельним методом.
 * @param[in] method Номер методу: 1 — половинне ділення; 2 — метод хорд.
 * @pre Значення method повинно дорівнювати 1 або 2.
 * @details Зчитує межі відрізка, абсолютну похибку, ліміт ітерацій та режим
 *          налагодження. Перевіряє умови застосування вибраного алгоритму.
 *          Після досягнення ліміту дозволяє виконати наступну групу кроків,
 *          продовжити без ліміту або зупинитися з поточним результатом.
 *          Для половинного ділення оцінює похибку за півдовжиною відрізка;
 *          для хорд — за відношенням |f(x)| до мінімального модуля f'.
 * @note Обидва чисельні методи реалізовані всередині цієї функції.
 *       Результат і статус завершення виводяться у стандартний потік
 *       виведення; функція не повертає числового значення кореня.
 */
void solve(int method)
{
    // Межі, похибка, значення f і f' на межах; vertex — мінімум f' для варіанта 10.
    double a, b, eps, fa, fb, da, db, vertex = -100.0 / 3.0;
    // Діапазон f', допоміжне значення, наближення x та дані нерухомого кінця хорди.
    double lower, upper, t, x, fx, fixed = 0, fixed_f = 0;
    // Попереднє наближення, нижня межа |f'| та накопичений час обчислень.
    double old, min_d = 0, seconds = 0;
    // Знаменник формули хорди та кандидат на наступний коректний стан.
    double denominator, next_x, next_fx;
    clock_t start;

    // Ліміт групи, режим друку, загальний і груповий лічильники ітерацій.
    int limit, debug, iterations = 0, batch = 0;

    // Режим без ліміту, причини завершення та вибір дії після вичерпання ліміту.
    int unlimited = 0, found = 0, stopped = 0, stalled = 0, action;

    // Зчитування та перевірка вхідних даних; NaN і нескінченність не допускаються.
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

    printf("Show each iteration? (1 - Yes, 0 - No): ");
    if (scanf("%d", &debug) != 1 || (debug != 0 && debug != 1))
    {
        printf("Incorrect debug mode.\n");
        return;
    }

    // Відлік для випадку кореня на межі; перед ітераціями таймер запускається заново.
    start = clock();
    fa = f(a);
    fb = f(b);

    // Захист від переповнення при обчисленні функції зі скінченним аргументом.
    if (!isfinite(fa) || !isfinite(fb))
    {
        printf("Function values are not finite. Choose another interval.\n");
        return;
    }

    // Обчислений нуль на межі обробляється без ітерацій і без заяви про єдиність.
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

    // Різні знаки гарантують існування кореня неперервної функції.
    // Однакові знаки не доводять відсутності коренів, але не задовольняють
    // умову ізоляції, яку використовують ці реалізації чисельних методів.
    if ((fa > 0 && fb > 0) || (fa < 0 && fb < 0))
    {
        printf("No sign change on the interval.\n");
        return;
    }

    // f'(x) — парабола: її діапазон визначається кінцями відрізка
    // та вершиною x = -100/3, якщо вона належить внутрішній частині відрізка.
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

    // Сталий ненульовий знак f' разом зі зміною знака f гарантує рівно один корінь.
    if (!(lower > 0 || upper < 0))
    {
        printf("Uniqueness of the root is not confirmed.\n");
        return;
    }
    printf("Exactly one root on the interval.\n");

    // Початковий стан; для хорд рухомий кінець за потреби змінюється нижче.
    x = a;
    fx = fa;

    if (method == 2)
    {
        // Для лінійної f'' однакові ненульові знаки на межах гарантують сталу опуклість.
        if (!((d2(a) > 0 && d2(b) > 0) ||
              (d2(a) < 0 && d2(b) < 0)))
        {
            printf("Choose an interval with constant sign of f''(x).\n");
            return;
        }

        // Нерухомим обирається кінець, для якого f*f'' > 0; добуток не обчислюється.
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

        // Мінімальний модуль похідної потрібен для оцінки |x - root| <= |f(x)|/min_d.
        min_d = fabs(da);
        if (fabs(db) < min_d) min_d = fabs(db);
        if (a < vertex && vertex < b && fabs(d1(vertex)) < min_d)
            min_d = fabs(d1(vertex));

        if (!isfinite(min_d) || min_d <= 0)
        {
            printf("Cannot estimate root accuracy.\n");
            return;
        }
    }

    // Початок вимірювання ітераційної частини обчислень.
    start = clock();

    while (!found && !stopped && !stalled)
    {
        // Призупинення перед наступним кроком після виконання дозволеної групи ітерацій.
        if (!unlimited && batch >= limit)
        {
            // Час очікування відповіді не входить до завершеної обчислювальної ділянки.
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

            // Поновлення відліку після відповіді, зокрема при помилці читання або EOF.
            start = clock();

            if (action == 3)
            {
                stopped = 1;
                break;
            }

            // Нова група зберігає наближення та загальний лічильник; дія 2 вимикає ліміт.
            if (action == 1) batch = 0;
            else unlimited = 1;
        }

        if (method == 1)
        {
            // Метод половинного ділення: наступне наближення — середина поточного відрізка.
            x = a + (b - a) / 2;
            fx = f(x);
        }
        else
        {
            // Метод хорд: перетин прямої через нерухому й рухому точки з віссю X.
            denominator = fixed_f - fx;
            if (denominator == 0 || !isfinite(denominator))
            {
                stalled = 1;
                break;
            }

            // Новий стан приймається лише після перевірки скінченності та належності відрізку.
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

            // Збереження попередньої точки для виявлення зупинки через округлення.
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
            // Гарантована оцінка похибки середини ізолювального відрізка.
            if (fx == 0 || (b - a) / 2 <= eps)
                found = 1;
            // Округлення середини до межі унеможливлює подальше звуження відрізка.
            else if (x == a || x == b)
                stalled = 1;
            // Залишається половина, на кінцях якої функція має різні знаки.
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
            // Оцінка абсолютної похибки за теоремою Лагранжа, а не лише за зміною x.
            if (fx == 0 || fabs(fx) / min_d <= eps)
                found = 1;
            // Зупинка без заяви про досягнення точності, якщо нове x вже не відрізняється.
            else if (x == old)
                stalled = 1;
        }
    }

    // Завершення вимірювання: додається остання ділянка; короткий час може округлитися до нуля.
    seconds += (double)(clock() - start) / CLOCKS_PER_SEC;

    // Виведення останнього наближення, нев'язки, кількості ітерацій, часу та причини завершення.
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

/**
 * @brief Точка входу в програму та організація головного меню.
 * @details Повторює вибір методу й викликає solve(). Значення 0 завершує
 *          роботу; інший непідтримуваний цілий номер спричиняє повтор меню.
 *          Неможливість прочитати ціле число також завершує програму.
 * @retval 0 Завершення роботи після вибору виходу або невдалого читання пункту меню.
 */
int main(void)
{
    int choice;

    for (;;)
    {
        printf("\n1 - Bisection method\n");
        printf("2 - Chord method\n");
        printf("0 - Exit\nChoose: ");

        // Непрочитані літери після помилки scanf не вилучаються і можуть завершити програму тут.
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
