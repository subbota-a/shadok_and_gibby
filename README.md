# Shadok and Gibby

![screenshot](doc/screenshot.png)

The game take over from the book
[Программирование игр и головоломок. Жак Арсак](https://coollib.in/b/181686-zhak-arsak-programmirovanie-igr-i-golovolomok/read),
game #8.

## Quote from the book (in Russian)

У шадоков ситуация удовлетворительна. Испытания ракет продолжаются, постоянно кончаясь неудачами.

Дело здесь в одном из основных принципов шадокской логики: «Нет ничего, что бы непрерывно продолжалось и не кончилось
успехом». Или, в других выражениях: «Чем больше неудач, тем больше шансов, что оно заработает». Их ракета еще
несовершенна, но они вычислили, что у них есть по крайней мере один шанс из миллиона, что она заработает… И они
торопятся поскорее осуществить 999999 первых неудачных опытов, чтобы быть уверенными, что миллионная заработает». (Жак
Руксель. Великолепие навыворот. Париж, издательство Грассе.)

Великий колдун сказал, что ракеты терпят неудачу потому, что не хватает транзисторов в системах безопасности. Но у гиби
транзисторы собирают с растений, произрастающих на огородах. Решено послать одного из шадоков на планету гиби искать
транзисторы. Гиби, очень умные благодаря своим шляпам, быстро проникли в планы шадоков и решили позабавиться. Они
позволили шадоку забраться в один из их огородов, но окружили его со всех сторон, и всякий раз, когда растение
расцветает и дает транзистор, они мчатся, чтобы собрать урожай прежде шадока.

## Description (in English)

You are Shadok and you are in the field with flowers and Gibbies.
You can move horizontally, vertically and diagonally on one step.
Your goal is to eat the flowers which bring the scores.
You have to score points within 100 steps.
The flowers bring different amount of scores, the brighter the more.
As soon as the flower is eaten, a new one grows at a random place.
Gibby prevents Shadok from eating flowers.

## Описание (in Russian)
Вы - Шадок и Вы на поле с цветами и Гибби. Вы можете двигаться горизонтально, вертикально и по диагонали.
Ваша цель - съесть цветы, чтобы набрать больше 100 очков и потратить меньше 100 ходов. 
Каждый цветок даёт очки в зависимости от его яркости - чем ярче тем больше.
Как только цветок съеден, вырастает новый на пустом месте.
Гибби Вам мешают собирать цветы.

## Keys

| Direction  | Numpad key | Arrow Key  |
|------------|------------|------------|
| Up         | 8          | Up         |
| Left       | 4          | Left       |
| Right      | 6          | Right      |
| Down       | 2          | Down       |
| Up-Left    | 7          | Up+Right   |
| Up-Right   | 9          | Up+Right   |
| Down-Left  | 1          | Down+Left  |
| Down-Right | 3          | Down+Right |

You can move only to an empty space.

## Config file

Config file `ShadokAndGibby.toml` is at:

- Windows: %LOCALAPPDATA%
- Linux: $HOME/.config

You can change the size of the field, the number of Gibbies, the number of colors, their points, the number of steps and
points to win.
