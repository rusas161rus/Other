import sympy

def bisection_method(a, b, eps, equation):
    x = sympy.Symbol('x')  # Определение символа x
    fa = equation.subs(x, a)
    fb = equation.subs(x, b)
    if fa == 0:
        return a
    if fb == 0:
        return b

    while abs(b - a) > eps:
        try:
            c = (a + b) / 2
            fc = equation.subs(x, c)
        except TypeError:
            print("Ошибка при вычислении функции. Проверьте правильность уравнения.")
            return None

        if sympy.sign(fa) * sympy.sign(fc) < 0:
            b = c
            fb = fc
        elif sympy.sign(fc) * sympy.sign(fb) < 0:
            a = c
            fa = fc
        else:
            break
    
    return c

while True:
    equation_input = input("Введите уравнение (или 'q' для выхода): ")
    if equation_input == 'q':
        break

    try:
        equation = sympy.sympify(equation_input)
    except sympy.SympifyError:
        print("Ошибка при вводе уравнения.")
        continue

    try:
        xn = float(input("Введите начало отрезка xn: "))
        xk = float(input("Введите конец отрезка xk: "))
        epsilon = float(input("Введите требуемую точность eps: "))
    except ValueError:
        print("Ошибка при вводе числовых данных.")
        continue

    root = bisection_method(xn, xk, epsilon, equation)
    if root is not None:
        print("Найден корень уравнения:", root)
