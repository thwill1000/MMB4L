' Copyright (c) 2021-2022 Thomas Hugo Williams
' License MIT <https://opensource.org/licenses/MIT>
' For MMBasic 5.07

Option Explicit On
Option Default None
Option Base 0

Print "Hello World: " Mm.CmdLine$
Dim s$ = Mm.Info(Current)

Execute("RUN " + Chr$(34) + s$ + Chr$(34) + ", --foo")
