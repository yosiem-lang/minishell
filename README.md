
# 🐚 Minishell: A Custom Shell Implementation

## 📋 プロジェクト概要

このプロジェクトは、**Unixシェル**（特にBash）の基本的な機能を再現するカスタムシェル「**Minishell**」をC言語で実装するものです。主要なビルトインコマンド、プロセス実行、パイプ、リダイレクション、環境変数管理、そしてシグナル処理などのコア機能が含まれています。

## 🚀 機能一覧 (Features)

Minishellは、以下の主要な機能をサポートしています。

### 1\. プロセス管理と実行

  * **コマンド実行**: 絶対パス (`/bin/ls`) および `$PATH`を通じたコマンド実行 (`ls`)。
  * **プロセス終了ステータス**: 最後の実行プロセスの終了コードを `$?` で取得。
  * **引数処理**: 複雑な引数と引用符の処理。

### 2\. リダイレクションとパイプ

  * **リダイレクション**:
      * 標準出力の上書き (`>`) および追記 (`>>`)
      * 標準入力のリダイレクト (`<`)
      * ヒアドキュメント (`<<`)
  * **パイプ**: 複数のコマンドを接続し、標準入出力を連携させる機能 (`cmd1 | cmd2 | cmd3`)。

### 3\. 環境変数と引用符

  * **環境変数展開**: ダブルクォート内 (`"Hello $USER"`) での変数展開をサポート。
  * **リテラル処理**: シングルクォート (`'...'`) 内の文字列は展開せず、リテラルとして扱う。

### 4\. ビルトインコマンド (Built-in Commands)

以下の標準的なビルトインコマンドを実装しています。

| コマンド | 機能 |
| :--- | :--- |
| **`echo`** | 文字列を出力。`-n` オプションをサポート。 |
| **`cd`** | カレントディレクトリを変更。相対パス、絶対パス、`..`, `.` をサポート。 |
| **`pwd`** | カレントディレクトリの絶対パスを出力。 |
| **`export`** | 環境変数を設定、変更、およびエクスポート。 |
| **`unset`** | 環境変数を削除。 |
| **`env`** | 環境変数を表示。 |
| **`exit`** | シェルを終了。引数による終了ステータス指定をサポート。 |

### 5\. シグナル処理

  * **Ctrl-C** (`SIGINT`): プロセスの中断と新しいプロンプトの表示。
  * \**Ctrl-\** (`SIGQUIT`): 実行中のコマンドを終了。（通常、プロンプトでは何もしない）
  * **Ctrl-D** (`EOF`): EOF（End of File）として解釈し、シェルを終了。

## 🧪 評価用テストケース (Evaluation Test Cases)

Minishellの動作を検証するための具体的なコマンド例です。

### 1\. シンプルコマンド & エラー処理

| テスト目的 | コマンド例 | 期待される動作 |
| :--- | :--- | :--- |
| **絶対パス実行** | `/bin/ls` | 実行される |
| **単純なコマンド** | `ls -l` | 実行される |
| **空のコマンド** | (Enterのみ) | プロンプト再表示 |
| **無効なコマンド** | `this_command_does_not_exist` | エラーメッセージ表示（クラッシュ禁止） |
| **終了ステータス** | `ls file_exists` & `echo $?` | `0` が表示される |
| **終了ステータス** | `ls file_not_exists` & `echo $?` | `1` または `2` が表示される |

### 2\. 引用符と環境変数

| テスト目的 | コマンド例 | 期待される出力 |
| :--- | :--- | :--- |
| **シングルクォート** | `echo '$USER'` | `$USER` (リテラル) |
| **ダブルクォート** | `echo "$USER"` | `$USER` の値 |
| **ダブルクォート内のリテラル** | `echo "pipe \| not pipe"` | `pipe | not pipe` |
| **変数展開** | `echo Hello $USER` | `Hello [ユーザー名]` |
| **空の変数** | `echo "$NON_EXISTING_VAR"` | (空行) |

### 3\. ビルトインコマンドの動作

| テスト目的 | コマンド例 | 期待される動作 |
| :--- | :--- | :--- |
| **`echo` の `-n`** | `echo -n hello world` | `hello world` (改行なし) |
| **`cd` の移動** | `cd ..` & `pwd` | 1つ上のディレクトリが表示される |
| **`export` 新規** | `export MY_VAR=42` & `env` | `MY_VAR=42` が表示される |
| **`unset` 削除** | `unset MY_VAR` & `env` | `MY_VAR` がリストから消える |
| **`exit` ステータス** | `exit 127` | シェルが終了し、`bash`で`echo $?`が`127` |
| **`exit` エラー** | `exit too many args` | エラー表示後、終了しない |

### 4\. リダイレクションとパイプ

| テスト目的 | コマンド例 | 期待される動作 |
| :--- | :--- | :--- |
| **リダイレクト** | `echo "test" > out.txt` | `out.txt`に`test`が書き込まれる |
| **追記** | `echo "append" >> out.txt` | `out.txt`の末尾に追記される |
| **入力** | `cat < out.txt` | `out.txt`の内容が出力される |
| **ヒアドキュメント** | `grep test << EOF` | EOFまで入力待ちとなり、入力が`grep`に渡される |
| **パイプ** | `ls -l | grep minish | wc -l` | 正しく実行され、最終結果が出力される |
| **パイプとリダイレクト** | `ls -l | grep minish > pipe_out.txt` | 結果がファイルに書き込まれる |

### 5\. シグナル処理のテスト

| テスト目的 | シグナル操作 | 状況 | 期待される動作 |
| :--- | :--- | :--- | :--- |
| **Ctrl-C (SIGINT)** | **Ctrl-C** | プロンプトが空の状態 | 新しい行にプロンプト再表示 |
| **Ctrl-C (実行中)** | **Ctrl-C** | `cat` 実行中 | `cat` が中断され、プロンプト再表示 |
| **Ctrl-\\ (SIGQUIT)** | \**Ctrl-\** | プロンプトが空の状態 | 何もしない |
| **Ctrl-\\ (実行中)** | \**Ctrl-\** | `cat` 実行中 | `Quit: 3` などが表示され、プロンプト再表示 |
| **Ctrl-D (EOF)** | **Ctrl-D** | プロンプトが空の状態 | シェルが終了する |

## 🛠️ ビルドと実行方法

1.  **リポジトリのクローン**

    ```bash
    git clone [リポジトリURL]
    cd minishell
    ```

2.  **コンパイル**

    ```bash
    make
    ```

      * コンパイルには `-Wall -Wextra -Werror` フラグが適用されています。

3.  **実行**

    ```bash
    ./minishell
    ```

    シェルが起動し、プロンプトが表示されます。

    ```
    minishell>
    ```



# 評価シート


Mandatory Part

Compile

    Use "make -n" to see if compilation use "-Wall -Wextra -Werror". If not, select the "invalid compilation" flag.
    minishell compiles without any errors. If not, select the flag.
    The Makefile must not re-link. If not, select the flag.

| テスト目的 | コマンド（Shellで実行） |
| :--- | :--- |
| ビルドフラグ確認 | `make -n` |
| コンパイルとエラー確認 | `make` |
| 再リンクの禁止確認 | `make` を連続で2回実行 |
| **起動** | `./minishell` (Minishellプロンプトへ) |

---

Simple Command & global variables

    Execute a simple command with an absolute path like /bin/ls, or any other command without any options.
    How many global variables are used? Why? Ask the evaluated student to give you a concrete example of why it feels mandatory or logical.
    Check the global variable. This global variable cannot provide any other information or data access than the number of a received signal.
    Test an empty command.
    Test only spaces or tabs.
    If something crashes, select the "crash" flag.
    If something doesn't work, select the "incomplete work" flag.

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **絶対パス実行** | `/bin/ls` | 外部コマンドが正しく実行されるか。 |
| **単純なコマンド** | `pwd` または `env` | ビルトインコマンドが正しく実行されるか。 |
| **外部コマンド** | `cat` | 別の外部コマンドが正しく実行されるか。 |
| **空のコマンド** | （Enterキーのみ） | 何も実行されず、クラッシュしないか。 |
| **スペース・タブのみ** | `     ` (スペース/タブのみ) | 何も実行されず、クラッシュしないか。 |

---

Arguments

    Execute a simple command with an absolute path like /bin/ls, or any other command with arguments but without any quotes or double quotes.
    Repeat multiple times with different commands and arguments.
    If something crashes, select the "crash" flag.
    If something doesn't work, select the "incomplete work" flag.

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **外部コマンドと単一引数** | `/bin/cat /etc/hosts` | 絶対パスで指定されたコマンドが単一の引数を処理できるか。 |
| **外部コマンドと複数引数** | `ls -l Makefile main.c` | `$PATH` 経由のコマンドが複数の引数とオプションを処理できるか。 |
| **ビルトインコマンドと引数** | `echo hello world` | ビルトインコマンドが引数を正しく受け取るか。 |
| **引数の順序確認** | `echo first second third` | 引数が入力順に処理されるか。 |
| **異なる外部コマンド** | `grep test Makefile` | 別の外部コマンドで引数処理が機能するか。 |
| **数字の引数** | `echo 1 2 3 4` | 数字を引数として処理できるか。 |
| **記号の引数** | `echo file-1 list_2` | ダッシュやアンダースコアを含む引数を処理できるか。 |
| **存在しないファイル** | `cat non_existent_file` | 引数が無効な場合、適切なエラーメッセージを表示し、**クラッシュしない**か。 |

---

echo

    Execute the echo command with or without arguments, or the -n option.
    Repeat multiple times with different arguments.
    If something crashes, select the "crash" flag.
    If something doesn't work, select the "incomplete work" flag.

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **基本的な実行** | `echo hello world` | `hello world` が表示され、最後に改行があるか。 |
| **引数なし** | `echo` | 空行が出力され、改行があるか。 |
| **`-n` オプション** | `echo -n hello` | `hello` が表示され、**改行がない**か。 |
| **`-n` と引数** | `echo -n hello world` | `hello world` が表示され、改行がないか。 |
| **`-n` の後の引数なし** | `echo -n` | 何も表示されず、改行がないか。 |
| **複数の `-n`** | `echo -n -n -n test` | `-n` の組み合わせが正しく解釈され、`test` が改行なしで表示されるか。 |
| **`-n` の後のテキスト** | `echo -n argument` | `argument` が改行なしで表示されるか。 |
| **他の引数との混在** | `echo hello -n world` | `-n` がオプションとして認識されず、`hello -n world` が表示され、改行があるか。 |
| **空の引数** | `echo ""` | 空行が表示され、改行があるか。（クォート処理の実装に依存するが、単純な引数処理として確認） |
| **数字と記号** | `echo 123 !@#$` | 数字や記号が正しく出力されるか。 |
| **`-n` が引数として扱われるケース** | `echo argument-n` | `-n` がオプションではなく、引数の一部として扱われるか。 |

---

exit

    Execute exit command with or without arguments.
    Repeat multiple times with different arguments.
    Don't forget to relaunch the minishell
    If something crashes, select the "crash" flag.
    If something doesn't work, select the "incomplete work" flag.

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **引数なしの終了** | `exit` | Minishell が終了するか。直前のコマンドのステータスで終了するか。 |
| **正常終了ステータス** | `exit 0` | Minishell が終了するか。終了後、親シェルで `echo $?` が **0** か。 |
| **異常終了ステータス** | `exit 127` | Minishell が終了するか。終了後、親シェルで `echo $?` が **127** か。 |
| **ステータスのモジュロ確認** | `exit 256` | Minishell が終了するか。終了後、親シェルで `echo $?` が **0** (256 % 256) か。 |
| **負のステータス** | `exit -1` | Minishell が終了するか。終了後、親シェルで `echo $?` が **255** か。 |
| **大きすぎる数値** | `exit 999999999999999999` | Minishell が終了し、ステータスが正しく計算されるか。 |
| **数字以外の引数** | `exit hello` | エラーメッセージ（例: `exit: hello: numeric argument required`）を表示し、ステータス **255** で終了するか。 |
| **複数の引数** | `exit 1 2 3` | エラーメッセージ（例: `exit: too many arguments`）を表示し、**終了しない**か。 |
| **スペースのみ** | ` exit      ` | スペースを無視し、引数なしと同じ動作で終了するか。 |

---

Return value of a process

    Execute a simple command with an absolute path like /bin/ls, or any other command with arguments but without any quotes and double quotes. Then execute echo $?
    Check the printed value. You can do the same in bash in order to compare the results.
    Repeat multiple times with different commands and arguments. Try some wrong commands like '/bin/ls filethatdoesntexist'
    Try anything like expr $? + $?
    If something crashes, select the "crash" flag.
    If something doesn't work, select the "incomplete work" flag.

### 1\. 正常終了 (ステータス 0) の確認

| テスト目的 | コマンド（Minishellで実行） | 期待される `$?` | 評価ポイント |
| :--- | :--- | :--- | :--- |
| **成功 (絶対パス)** | `/bin/ls /` | `0` | `/bin/ls` が正常に終了するか。 |
| **成功 (引数なし)** | `echo hello` | `0` | ビルトインコマンドが正常終了するか。 |
| **成功 (外部コマンド)** | `grep minish Makefile` | `0` | 検索に成功した場合、`0` が返るか。 |

### 2\. 異常終了 (ステータス \> 0) の確認

| テスト目的 | コマンド（Minishellで実行） | 期待される `$?` | 評価ポイント |
| :--- | :--- | :--- | :--- |
| **ファイルが存在しない** | `/bin/ls filethatdoesntexist` | `1` または `2` | ファイルが見つからないエラーで、非ゼロの値が返るか。（Bashの動作と比較） |
| **コマンドが見つからない** | `non_existent_command` | `127` | `$PATH` のどこにもコマンドがない場合、`127` が返るか。 |
| **パーミッションエラー** | `cat /etc/shadow` | `1` | 実行権限や読み取り権限がない場合、非ゼロの値が返るか。 |
| **grep 失敗** | `grep non_existent_string Makefile` | `1` | 検索に失敗した場合、`1` が返るか。 |

### 3\. 特殊なテスト (`$` 展開の確認)

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **`$?` の展開** | `expr 1 + 1` の後 `echo $?` | `expr` が正常終了（`0`）した後に、`$?` が正しく展開されるか。 |
| **連続展開 (任意)** | `echo $? + $?` | `echo` が実行される前に `$?` がリテラルとして展開されるか。 |
| **二重展開の確認** | `ls wrong_file` の後 `echo "status is $?"` | ダブルクォート内で `$?` が正しく展開されるか。 |

---

Signals

    ctrl-C in an empty prompt should display a new line with a new prompt.
    ctrl-\ in an empty prompt should not do anything.
    ctrl-D in an empty prompt should quit minishell --> RELAUNCH!
    ctrl-C in a prompt after you wrote some stuff should display a new line with a new prompt.
    The buffer should be clean too. Press "Enter" to make sure nothing from the previous line is executed.
    ctrl-D in a prompt after you wrote some stuff should not do anything.
    ctrl-\ in a prompt after you wrote some stuff should not do anything.
    Try ctrl-C after running a blocking command like cat without arguments or grep “something“.
    Try ctrl-\ after running a blocking command like cat without arguments or grep “something“.
    Try ctrl-D after running a blocking command like cat without arguments or grep “something“.
    Repeat multiple times using different commands.
    If something crashes, select the "crash" flag.
    If something doesn't work, select the "incomplete work" flag.

### 1. ⌨️ プロンプト入力状態でのシグナルテスト

| テスト目的 | コマンド/操作（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **Ctrl-C (空プロンプト)** | (何も入力せず) **Ctrl-C** | 新しい行にプロンプトが表示され、クラッシュしないか。 |
| **Ctrl-\ (空プロンプト)** | (何も入力せず) **Ctrl-\** | **何も表示されず**、プロンプトが維持されるか。 |
| **Ctrl-D (空プロンプト)** | (何も入力せず) **Ctrl-D** | Minishell が終了するか。**⇒ 終了したら再起動** |
| **Ctrl-C (入力途中)** | `echo test` と入力後、**Ctrl-C** | 入力行がキャンセルされ、新しい行にプロンプトが表示されるか。 |
| **バッファクリア確認** | `echo test` と入力後、**Ctrl-C**、次に **Enter** | 前の入力 (`echo test`) が**実行されない**ことを確認。 |
| **Ctrl-D (入力途中)** | `echo test` と入力後、**Ctrl-D** | 何も実行されず、プロンプトが維持されるか。 |
| **Ctrl-\ (入力途中)** | `echo test` と入力後、**Ctrl-\** | 何も実行されず、プロンプトが維持されるか。 |

### 2. 🏃 実行中のシグナルテスト（ブロッキングコマンド使用）

| テスト目的 | コマンド（Minishellで実行） | 操作 | 評価ポイント |
| :--- | :--- | :--- | :--- |
| **Ctrl-C (実行中)** | `cat` | `cat` 実行中に **Ctrl-C** を押す | `cat` が**中断**され、新しいプロンプトが表示されるか。 ($? は通常 **130** か) |
| **Ctrl-\ (実行中)** | `cat` | `cat` 実行中に **Ctrl-\** を押す | `cat` が**中断**され、「Quit: 3」などが出力され、新しいプロンプトが表示されるか。 ($? は通常 **131** か) |
| **Ctrl-D (実行中)** | `cat` | `cat` 実行中に **Ctrl-D** を押す | `cat` に EOF が送信され、**cat が正常終了**し、プロンプトが戻るか。 ($? は通常 **0** か) |
| **別コマンドで繰り返し** | `grep "something"` | `grep` 実行中に **Ctrl-C** | 同様に中断され、クラッシュしないか。 |

---

Double Quotes

    Execute a simple command with arguments and, this time, use also double quotes (you should try to include whitespaces too).
    Try a command like : echo "cat lol.c | cat > lol.c"
    Try anything except $.
    If something crashes, select the "crash" flag.
    If something doesn't work, select the "incomplete work" flag.

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **空白を含む引数** | `echo "hello world"` | `hello world` が単一の引数として正確に出力され、`echo` が引数を複数と認識しないか。 |
| **特殊文字のリテラル化 (パイプ)** | `echo "cat lol.c | cat"` | `|` がパイプとして解釈されず、文字列としてそのまま出力されるか。 |
| **特殊文字のリテラル化 (リダイレクト)** | `echo " > file.txt"` | `>` がリダイレクションとして機能せず、文字列として出力されるか。 |
| **特殊文字のリテラル化 (セミコロン)** | `echo "command; other"` | `;` がコマンドセパレータとして機能せず、文字列として出力されるか。 |
| **空のダブルクォート** | `echo ""` | 空行が出力されるか。 |
| **引数間の空白** | `echo "arg1" "arg2"` | `arg1` と `arg2` がスペース区切りで正しく出力されるか。 |
| **引数の一部に空白** | `echo first" second "third` | `first second third` が正しく単語分割され、出力されるか。 |

---

Single Quotes

    Execute commands with single quotes as arguments.
    Try empty arguments.
    Try environment variables, whitespaces, pipes, redirection in the single quotes.
    echo '$USER' must print "$USER".
    Nothing should be interpreted.

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **環境変数のリテラル化** | `echo '$USER'` | **$USER** という文字通りに出力されるか。 |
| **パイプのリテラル化** | `echo 'ls -l | wc -l'` | `|` がパイプとして解釈されず、文字列としてそのまま出力されるか。 |
| **リダイレクションのリテラル化** | `echo ' > file.txt '` | `>` がリダイレクションとして機能せず、文字列として出力されるか。 |
| **空白の保持** | `echo '  leading and trailing spaces  '` | クォート内の**先頭と末尾の空白**が保持されて出力されるか。 |
| **空の引数** | `echo ''` | 空行が出力されるか。 |
| **複数の引数** | `echo 'arg1' 'arg2'` | `arg1` と `arg2` がスペース区切りで正しく出力されるか。 |
| **複合テスト** | `echo '$PATH"|">$USER'` | クォート内の特殊文字やダブルクォートも全てリテラルとして出力されるか。 |

---

env

    Check if env shows you the current environment variables.

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **基本的な実行** | `env` | Minishell が起動時に受け継いだ**全ての環境変数**が、`KEY=VALUE` 形式で正しく表示されるか。 |
| **引数付き** (Bash互換性テスト) | `env test` | Bash と同様に、エラーメッセージを表示するか、または引数を無視して `env` の内容を表示し、クラッシュしないか。（Minishellの仕様に依存） |

---

export

    Export environment variables, create new ones and replace old ones.
    Check the result with env.

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **新規変数の作成** | `export NEW_VAR=42` | `NEW_VAR` が環境変数として作成されるか。 |
| **作成結果の確認** | `env` | 出力に `NEW_VAR=42` が含まれているか。 |
| **既存変数の上書き** | `export NEW_VAR=hello` | 既存の `NEW_VAR` の値が `hello` に更新されるか。 |
| **上書き結果の確認** | `env` | 出力に `NEW_VAR=hello` が含まれているか。 |
| **値のない変数の作成** | `export EMPTY_VAR` | `EMPTY_VAR` が作成され、`env` には表示されないが、Minishell 内部では追跡されているか。 |
| **無効な変数名** | `export 1VAR=error` | エラーメッセージが表示され、**クラッシュしない**か。変数が作成されないか。 |
| **値にスペースを含む変数** | `export QUOTED_VAL="with space"` | 値が正しくクォートされて Minishell 内部に保持されるか。 ( `env` の出力は `with space` となるべき) |
| **空の値で上書き** | `export NEW_VAR=""` | `NEW_VAR` の値が空文字列に更新されるか。 |

---

unset

    Export environment variables, create new ones and replace old ones.
    Use unset to remove some of them.
    Check the result with env.

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **準備：新規変数の作成** | `export VAR_TO_REMOVE=test_value` | 削除対象の変数を環境に設定する。 |
| **準備：作成結果の確認** | `env` | `VAR_TO_REMOVE` が表示されることを確認する。 |
| **変数の削除** | `unset VAR_TO_REMOVE` | `VAR_TO_REMOVE` が環境から削除されるか。 |
| **削除結果の確認** | `env` | `VAR_TO_REMOVE` がリストに**含まれていない**ことを確認する。 |
| **存在しない変数の削除** | `unset NON_EXISTENT_VAR` | エラーメッセージが表示されず、**クラッシュしない**か。 |
| **複数の変数の削除** | `export A=1 B=2; unset A B` | 複数の変数を一度に削除できるか。 |
| **$PATH の削除** | `unset PATH` | `PATH` 変数が削除されるか。 |
| **削除後のコマンド実行** | `ls` | `PATH` 削除後、`ls` などの外部コマンドが実行できなくなるか。 |
| **無効な変数名** | `unset 1VAR` | エラーメッセージが表示され、**クラッシュしない**か。 |

---

cd

    Use the command cd to move the working directory and check if you are in the right directory with /bin/ls
    Repeat multiple times with working and not working cd
    Also, try '.' and '..' as arguments.

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **絶対パスへの移動** | `cd /tmp` | ディレクトリが `/tmp` に変更されるか。 |
| **移動結果の確認** | `/bin/pwd` | `/tmp` が表示されるか。 |
| **ホームディレクトリへの移動** | `cd` | `$HOME` に設定されたディレクトリに移動するか。 |
| **`.` (カレントディレクトリ) へ移動** | `cd /usr/bin` の後 `cd .` | ディレクトリが変更されないか。 |
| **`..` (親ディレクトリ) へ移動** | `cd /tmp/test_dir` の後 `cd ..` | 1つ上のディレクトリ（`/tmp`）に移動するか。 |
| **複雑な相対パス** | `cd ../../etc/` | 複数の `..` やディレクトリ名を含む相対パスが正しく解決されるか。 |
| **存在しないディレクトリ** | `cd /nonexistent/path` | エラーメッセージが表示され、**ディレクトリが変更されない**か。 |
| **ファイル名** | `cd /etc/hosts` | `hosts` はディレクトリではないため、エラーが表示され、ディレクトリが変更されないか。 |
| **引数なしの検証** | `/bin/ls` | `cd` のテスト後に、カレントディレクトリを基準とした `/bin/ls` の結果が期待通りか。 |
| **複数の引数** | `cd /tmp /etc` | Bashと同様に、エラーメッセージを表示し、ディレクトリが変更されないか。 |

---

pwd

    Use the command pwd.
    Repeat multiple times in different directories.

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **基本的な実行** | `pwd` | 現在のディレクトリの絶対パスが正しく表示されるか。 |
| **ルートディレクトリでの実行** | `cd /` の後 `pwd` | `/` が表示されるか。 |
| **ホームディレクトリでの実行** | `cd` の後 `pwd` | ホームディレクトリの絶対パスが表示されるか。 |
| **サブディレクトリへの移動後** | `cd /tmp` の後 `pwd` | `/tmp` が表示されるか。 |
| **親ディレクトリへの移動後** | `cd ..` の後 `pwd` | 正しい親ディレクトリのパスが表示されるか。 |
| **引数付き実行** | `pwd argument` | 引数を無視してカレントディレクトリが表示されるか、または Bash と同様にエラーが発生し、クラッシュしないか。 |
| **環境変数** | `echo $PWD` | `$PWD` 環境変数の値が `pwd` の出力と同じか。（オプションの確認点） |

---

Relative Path

    Execute commands but this time use a relative path.
    Repeat multiple times in different directories with a complex relative path (lots of ..).

### 1. 準備

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **テスト用ディレクトリ作成** | `mkdir -p test_dir/sub_dir` | テスト環境を準備する。 |
| **テスト用ファイル作成** | `echo "Hello Minishell" > test_dir/file.txt` | テスト用ファイルを作成する。 |
| **作業ディレクトリ移動** | `cd test_dir` | 基準ディレクトリに移動する。 |

### 2. 相対パスの実行テスト

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **カレントディレクトリ (`./`) 実行** | `cat ./file.txt` | `file.txt` の内容が出力されるか。 |
| **サブディレクトリ内ファイルの実行** | `cat sub_dir/../file.txt` | 相対パスが正しく解決され、内容が出力されるか。 |
| **親ディレクトリ (`../`) 実行** | `cd sub_dir` の後 `cat ../file.txt` | 親ディレクトリのファイルが参照できるか。 |
| **複雑な相対パス (多数の `../`)** | `cd /usr/bin` の後 `ls ../../home` | 多数の `../` を含むパスが正しく解決されるか。（必要に応じて `/home` の代わりに `/` を使うなど、存在するディレクトリに調整） |
| **コマンド自体の相対パス実行** | `cd sub_dir` の後 `../file.txt` | `file.txt` を実行しようとして、適切なエラーメッセージ（ファイルは実行可能ではない、など）が表示され、クラッシュしないか。 |
| **無効な相対パス** | `cat ./nonexistent_dir/file.txt` | パス解決に失敗し、適切なエラーメッセージが表示されるか。 |

### 3. テスト後のクリーンアップ

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **ディレクトリ移動** | `cd ../../` (元の場所に戻る) | テスト後の位置を元に戻す。 |
| **ディレクトリ削除** | `rm -rf test_dir` | テストディレクトリを削除する。 |

---

Environment path

    Execute commands but this time without any path (ls, wc, awk and so forth).
    Unset the $PATH and ensure commands are not working anymore.
    Set the $PATH to a multiple directory value (directory1:directory2) and ensure that directories are checked in order from left to right.

### 1. `$PATH` の基本動作確認

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **$PATH 経由の実行** | `ls` | `$PATH` を通じて `/bin/ls` などが正しく実行されるか。 |
| **別コマンドの実行** | `wc` | 別の一般的な外部コマンドも実行できるか。 |
| **$PATH の確認** | `echo $PATH` | 現在の `$PATH` が正しく表示されるか。 |

### 2. `$PATH` の削除（unset）

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **$PATH の削除** | `unset PATH` | `$PATH` 環境変数が削除されるか。 |
| **削除後のコマンド実行** | `ls` | コマンドが見つからず、**「command not found」**などのエラーメッセージが表示され、クラッシュしないか。 |

### 3. `$PATH` の検索順序確認

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **準備：テスト用ディレクトリ作成** | `mkdir -p test_dir1 test_dir2` | テスト環境を準備する。 |
| **準備：コマンドファイル作成 (1)** | `echo 'echo from dir 1' > test_dir1/test_cmd` | ディレクトリ 1 にコマンドを作成する。 |
| **準備：コマンドファイル作成 (2)** | `echo 'echo from dir 2' > test_dir2/test_cmd` | ディレクトリ 2 に同名コマンドを作成する。 |
| **準備：実行権限付与** | `chmod +x test_dir1/test_cmd test_dir2/test_cmd` | コマンドファイルに実行権限を与える。 |
| **$PATH の設定 (1)** | `export PATH=./test_dir1:./test_dir2:$PATH` | `test_dir1` を優先して `$PATH` に設定する。 |
| **検索順序の確認** | `test_cmd` | **「echo from dir 1」**が出力されるか。（左側のディレクトリが優先されるか） |
| **$PATH の設定 (2)** | `export PATH=./test_dir2:./test_dir1:$PATH` | 順序を逆にして `$PATH` を再設定する。 |
| **検索順序の再確認** | `test_cmd` | **「echo from dir 2」**が出力されるか。（左側のディレクトリが優先されるか） |

### 4. テスト後のクリーンアップ

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **$PATH のリセット (任意)** | `export PATH=[元のPATH]` | 削除した `$PATH` を元に戻す。（手動で実行） |
| **テストディレクトリ削除** | `rm -rf test_dir1 test_dir2` | テスト環境を削除する。 |

---

Redirection

    Execute commands with redirections < and/or >
    Repeat multiple times with different commands and arguments and sometimes change > with >>
    Check if multiple tries of the same redirections fail.
    Test << redirection (it doesn't have to update the history).

### 1. 準備

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **テスト用ファイルの作成** | `echo initial content > input_file.txt` | `input_file.txt` が作成され、初期内容が含まれているか。 |

### 2. 基本的なリダイレクション (`>`, `>>`, `<`)

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **出力上書き (`>`)** | `echo new content > output_file.txt` | `output_file.txt` が新規作成され、`new content` のみ含まれているか。 |
| **上書き結果の確認** | `cat output_file.txt` | 結果を確認する。 |
| **出力追記 (`>>`) (1)** | `echo first line >> append_file.txt` | `append_file.txt` が作成され、`first line` が含まれるか。 |
| **出力追記 (`>>`) (2)** | `echo second line >> append_file.txt` | `second line` が**既存の内容の後に追記**されているか。 |
| **追記結果の確認** | `cat append_file.txt` | 2行とも表示されるか。 |
| **入力リダイレクト (`<`)** | `cat < input_file.txt` | `input_file.txt` の内容が `cat` の標準入力として渡され、出力されるか。 |
| **入力ファイルなし** | `cat < nonexistent_file.txt` | ファイルが存在しない場合、適切なエラーメッセージが表示され、クラッシュしないか。 |

### 3. 複合リダイレクションとエラー

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **入出力の複合** | `grep content < input_file.txt > grep_output.txt` | 入力と出力が同時に正しく処理され、`grep_output.txt` が作成されるか。 |
| **複数の出力リダイレクト** | `echo test > file1.txt > file2.txt` | **最後のファイル** (`file2.txt`) にのみ書き込まれ、クラッシュしないか。（Bashの挙動に準拠） |
| **パーミッションエラー** | `echo fail > /etc/shadow` | 書き込み権限エラーが発生し、適切なエラーメッセージが表示され、クラッシュしないか。 |

### 4. ヒアドキュメント (`<<`)

| テスト目的 | コマンド（Minishellで実行） | 操作 | 評価ポイント |
| :--- | :--- | :--- | :--- |
| **基本的なヒアドキュメント** | `cat << EOF` | 複数行入力後、**EOF**（区切り文字）のみの行で終了 | 入力した内容が `cat` の標準入力として渡され、出力されるか。 |
| **複数行の入力** | `grep test << END_MARKER` | 複数行入力（例: `line one`, `test line`, `line three`）後、**END\_MARKER** で終了 | `test line` のみが `grep` によって出力されるか。 |
| **区切り文字にスペース** | `cat << " END "` | 区切り文字にスペースを含んだ場合も正しく機能するか。 |
| **ヒストリの確認** | `cat << EOF` を実行後、**上矢印キー** | ヒアドキュメントの入力内容が**ヒストリに保存されていない**か。 |

### 5. テスト後のクリーンアップ

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **テストファイルの削除** | `rm input_file.txt output_file.txt append_file.txt grep_output.txt` | 作成したファイルを削除する。 |

---

Pipes

    Execute commands with pipes like 'cat file | grep bla | more'
    Repeat multiple times with different commands and arguments.
    Try some wrong commands like 'ls filethatdoesntexist | grep bla | more'
    Try to mix pipes and redirections.

### 1. 準備

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **テスト用ファイルの作成** | `echo "minishell\nhello\nminishell" > pipe_test_file.txt` | テスト用ファイルを作成する。 |

### 2. 基本的なパイプの動作確認

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **単純なパイプ (2プロセス)** | `cat pipe_test_file.txt | wc -l` | ファイルの行数（3行）が正しく出力されるか。 |
| **複数パイプ (3プロセス)** | `cat pipe_test_file.txt | grep mini | wc -l` | `grep` でフィルタリングされた後の行数（2行）が正しく出力されるか。 |
| **オプション付きコマンド** | `ls -l | head -n 3` | コマンドオプションが正しく処理され、パイプが機能するか。 |
| **引数付きコマンド** | `echo "a b c" | tr ' ' '\n'` | `tr` コマンドが引数と標準入力を正しく処理し、改行に変換されるか。 |

### 3. エラー処理とリダイレクションの混合

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **最初のコマンドが失敗** | `ls filethatdoesntexist | grep bla` | `ls` はエラーを出すが、**`grep` は実行され**、クラッシュしないか。 |
| **途中のコマンドが失敗** | `echo test | bad_command | wc -l` | `bad_command` がエラーを出すが、Minishell全体としてクラッシュしないか。 |
| **パイプと出力リダイレクトの混合** | `cat pipe_test_file.txt | grep mini > pipe_output.txt` | パイプの最終結果が `pipe_output.txt` に書き込まれるか。 |
| **パイプと入力リダイレクトの混合** | `grep mini < pipe_test_file.txt | wc -l` | `<` と `|` の両方が正しく機能し、期待通りの結果（2行）が出るか。 |
| **複雑な混合** | `ls -l | head -n 5 > output_pipe.txt | cat -e` | パイプとリダイレクションが複雑に組み合わされても処理されるか。 |

### 4. テスト後のクリーンアップ

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **テストファイルの削除** | `rm pipe_test_file.txt pipe_output.txt output_pipe.txt` | 作成したファイルを削除する。 |

---

Go Crazy and history

    Type a command line, then use ctrl-C and press "Enter". The buffer should be clean and there should be nothing left to execute.
    Can we navigate through history using Up and Down? Can we retry some command?
    Execute commands that should not work like 'dsbksdgbksdghsd'. Ensure minishell doesn't crash and prints an error.
    'cat | cat | ls' should behave in a "normal way".
    Try to execute a long command with a ton of arguments.
    Have fun with that beautiful minishell and enjoy it!

### 1. 🤯 シェルの堅牢性とバッファ制御

| テスト目的 | コマンド/操作（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **コマンドバッファのクリア** | `echo test` と入力後、**Ctrl-C**、次に **Enter** | `echo test` が**実行されず**、バッファが完全にクリアされているか。 |
| **未定義コマンドの処理** | `dsbksdgbksdghsd` | エラーメッセージ（例: `command not found`）が表示され、**Minishellがクラッシュしない**か。 |
| **長いコマンドライン** | `echo a b c d e f g h i j k l m n o p q r s t u v w x y z` | 非常に多数の引数を持つコマンドラインが正しく処理され、出力されるか。 |
| **変則的なパイプ** | `cat | cat | ls` | エラーやクラッシュなく、正常に処理され、最終的に `ls` が実行されるか。 |

### 2. 📜 ヒストリ機能の確認

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **ヒストリの蓄積 (1)** | `ls -l` | コマンドを実行し、ヒストリに保存させる。 |
| **ヒストリの蓄積 (2)** | `pwd` | コマンドを実行し、ヒストリに保存させる。 |
| **ヒストリナビゲーション** | **上矢印キー** (複数回) / **下矢印キー** | 実行した `pwd` や `ls -l` コマンドがヒストリから呼び出され、ナビゲートできるか。 |
| **コマンドの再実行** | **上矢印キー**で過去のコマンドを表示後、**Enter** | 呼び出したコマンドがヒストリから再実行できるか。 |

### 3. 複合的なエッジケース

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **シグナルとヒストリ** | `cat` を実行中に **Ctrl-C** | 中断後、`cat` がヒストリに保存されているか（通常は保存されないが、実装依存）。 |
| **複合エラー** | `echo $? | bad_command` | `$?` の展開、パイプ、エラー処理が同時に機能し、クラッシュしないか。 |

---

Environment variables

    Execute echo with some environment variables ($variable) as arguments.
    Check that $ is interpreted as an environment variable.
    Check that double quotes interpolate $.
    Check that USER exists. Otherwise, set it.
    echo "$USER" should print the value of the USER variable.

### 1. `$USER` の存在確認と設定

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **`USER` 変数の存在確認** | `echo $USER` | 通常、ユーザー名が出力されるか。 |
| **`USER` の値設定** | `export USER=minishell_user` | `USER` が設定されていない場合、またはテストのために値を変更する。 |

### 2. 環境変数展開のテスト

| テスト目的 | コマンド（Minishellで実行） | 評価ポイント |
| :--- | :--- | :--- |
| **非引用符での展開** | `echo Hello $USER` | `$USER` の値に置き換わって出力されるか。 |
| **ダブルクォート内での展開** | `echo "The user is $USER"` | ダブルクォート内で `$USER` が正しく展開されるか。 |
| **展開と隣接文字列** | `echo ${USER}name` | 変数名と後続の文字列が正しく区別され、展開されるか。（Bashの `$変数名` の基本機能を確認） |
| **展開と特殊文字** | `echo $USER$PATH` | 複数の変数が連続して展開されるか。 |
| **存在しない変数の展開** | `echo value is $NON_EXISTENT_VAR` | 存在しない変数は空文字列として扱われ、エラーにならず、クラッシュしないか。 |
| **$ のリテラル化** | `echo \$USER` | バックスラッシュでエスケープされた `$USER` は、そのまま `$USER` として出力されるか。 |
| **`$` 単体** | `echo $` | `$ `（単一のスペース）として扱われるか。 |

---

