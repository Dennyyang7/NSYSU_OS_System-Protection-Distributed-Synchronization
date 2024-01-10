目標：

這份作業主要著重於實現系統保護的概念，特別是關於檔案存取權限，透過在電腦之間建立TCP/IP連線。
檔案權限：

模擬UNIX檔案系統的權限，包括擁有者、群組成員和其他人的讀取、寫入和執行權限。
例如：-rw-r----- Ken AOS 97664 Nov 16 2023 homework2.c
伺服器和客戶端群組：

檔案操作、檔案創建、檔案存取

客戶端可以根據其權限下載或上傳檔案。
權限處理：
如果客戶端嘗試沒有必要權限的操作，伺服器應該禁止並顯示相應的訊息。
例如權限檢查的指令：read homework2.c（只有當權限允許時才會被允許）
動態模式更改：

客戶端可以動態更改檔案的存取模式。
例如更改模式的指令：changemode homework2.c rw----
實現注意事項：

實現一個處理檔案操作和執行權限檢查的伺服器。
客戶端通過TCP/IP連線與伺服器互動。
著重於安全性和正確處理檔案權限，以模擬UNIX檔案系統的行為。
