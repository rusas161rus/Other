from cmath import sqrt
import math

while True:
    try:        
        R = int(input("Введите Радиус верхнего основания R: \n"))
        r = int(input("Введите Радиус нижнего основания r: \n"))
        h = int(input("Введите Высота усеченного конуса h: \n"))  
                      
    except ValueError:
        print("Введите R: ")
        print("Введите r: ")
        print("Введите h: ")        
        continue

    if R<=0:
        print("Радиус R не может быть равен 0\n")
        continue
    if R>r:
        print("Радиус R должен быть меньше радиуса r\n")
        continue           
    if r<=0:
        print("Радиус r не может быть равен 0\n")
        continue
    if r<R:
        print("Радиус r должен быть больше радиуса R\n")
        continue
    if h<=0:
        print("Высота h не может быть равен 0\n")
        continue  

        
    if R>=1 and r>=1 and h>=1:
        sqrt=math.sqrt((R-r)**2+h**2)        
        S=math.pi*(R+r)*sqrt
        V=1/3*math.pi*h*(R**2+R*r+r**2)
        break

print(V, S)
