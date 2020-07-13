rsyslogをリモートログサーバーに設定する

1. /etc/rsyslog.confを編集してTCP経由の入力を受け付ける設定にする
module(load="imtcp")
input(type="imtcp" port="514")

2. Nlogクライアントをどのファシリティに当てはめるか決めてそのファシリ
   ティを指示する数値NをNLOGメッセージに'<N>'で埋め込む。例え
   ば'local0.alert'であれば<129>という文字列を先頭に埋め込むことになる。

* local0~7の番号は下記を参照のこと。
https://knowledge.sakura.ad.jp/8969/

3. /etc/rsyslog.d/yy-xxx.confを作成してNLOGが発行したログメッセージの
   格納の仕方を記述する。



