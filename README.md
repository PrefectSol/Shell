# Shell / Командный интерпретатор / OS Unix

<h3>Operations (the priority is the same on one line)</h3>
<p>
|    >    >>    < </br>
&&    ||</br>
;    &</br>
Parentheses () - changing the order of priorities 
</p>

<h3> Doc (RU)</h3>
<p>
<strong>pr1 | …| prN</strong> – конвейер: стандартный вывод всех команд, кроме последней, направляется
на стандартный ввод следующей команды конвейера. Каждая команда выполняется как
самостоятельный процесс (т.е. все pri выполняются параллельно). Shell ожидает
завершения последней команды. </br>
<strong>com1 ; com2</strong> – означает, что команды будут выполняться последовательно.</br>
<strong>com &</strong> - запуск команды в фоновом режиме (т.е. Shell готов к вводу следующей команды,
не ожидая завершения данной команды com, а com не реагирует на сигналы завершения,
посылаемые с клавиатуры, например, на нажатие Ctrl-C ).</br>
<strong>com1 && com2</strong> - выполнить com1, если она завершилась успешно, выполнить com2; </br>
<strong>com1 || com2</strong> - выполнить com1, если она завершилась неуспешно, выполнить com2.</br>

<h2>Перенаправление ввода-вывода</h2>
<strong>< файл</strong> - файл используется в качестве стандартного ввода; </br>
<strong>> файл</strong> - стандартный вывод направляется в файл (если файла не было - он создается,
если файл уже существовал, то его старое содержимое отбрасывается, т.е. происходит
вывод с перезаписью);</br>
<strong>>> файл</strong> – стандартный вывод направляется в файл (если файла не было - он создается,
если файл уже существовал, то его старое содержимое сохраняется, а запись производится
в конец файла)</br>
</p>