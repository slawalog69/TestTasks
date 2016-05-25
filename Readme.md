
###Description
--------------
Чтение времени из встроенного модуля часов реального времени. Измерение температуры кристалла встроенным датчиком температуры. Вывод замеров температуры (скользящее среднее из 8 замеров) через последовательный интерфейс UART в терминальную программу на компьютере со штампом времени раз в секунду.  Прекращение и возобновление вывода температуры в UART по нажатию кнопки.Измерение расстояния ультразвуковым датчиком. Вывод результатов на светодиодную шкалу из 4х светодиодов  [STM32F4Discovery](http://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-eval-tools/stm32-mcu-eval-tools/stm32-mcu-discovery-kits/stm32f4discovery.html) по полметра на каждый.

used Standard Peripherals Library V1.0.0 от STMicroelectronics

###root test_tasks:
------------------
- **ADC_BSP.c**  -  измерение температуры кристалла

-  **Button.c**  -   опрос состояния кнопки и работа св диодов `#define BUTTON_CHECK_ON_INTRRPT` - контроль кнопки по прерыванию
`#else` - контроль отдельной задачей

- **Irq.c**     -   переопределения обработчиков EXTI0 и USART2

- **mainTasks.c** - Инициализация и основная `task` -вывод результатов в PC

- **my_HCSR.c**  - работа с датчиком HC SR-04 Выбор комутации в **my_HCSR.h**. Текущее: Trigg pin - PB6, Echo pin - PB7

![HC SR-04](http://robocraft.ru/files/sensors/Ultrasonic/HC-SR04/front.jpg)

 
- **RTC_BSP.c** - работа с таймером реального времени. Часы инициализиуются: Пятница 31 декабря 15 года 23:59:50
 
- **USART_BSP.c** -  работа с COM портом. Выбор параметров USART в **USART_BSP.h**. Текущее: USART2, 115200, RxPin - PA3, TxPin - PA2
 
- **main.c** - пример main-файла FreeRTOS

------------------------------------
###Tool.exe 
- программа под Win для тестирования
