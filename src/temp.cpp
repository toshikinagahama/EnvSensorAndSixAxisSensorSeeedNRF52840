// #include <stdio.h>
// #include <stdlib.h> // malloc, free に必要

// // --- メイン状態定義 ---
// typedef enum
// {
//   MAIN_STATE_A,
//   MAIN_STATE_B,
//   NUM_MAIN_STATES
// } MainState;

// // --- サブ状態定義 ---
// typedef enum
// {
//   SUB_STATE_X,
//   SUB_STATE_Y,
//   NUM_SUB_STATES
// } SubState;

// // --- イベントビット定義 ---
// #define EVENT_NONE 0x00
// #define EVENT_GREET 0x01
// #define EVENT_GOODBYE 0x02
// #define EVENT_UNKNOWN 0x04

// // --- イベントビット定義 ---
// // typedef enum
// //{
// //  // イベントビットは通常、0から始まる2のべき乗で定義されます
// //  // 例: 1 << 0 は 0x01, 1 << 1 は 0x02, 1 << 2 は 0x04 ...
// //  EVENT_NONE = 0,           // イベントなし (0x00)
// //  EVENT_GREET = (1 << 0),   // 1を0ビット左にシフト -> 0001 (0x01)
// //  EVENT_GOODBYE = (1 << 1), // 1を1ビット左にシフト -> 0010 (0x02)
// //  EVENT_UNKNOWN = (1 << 2), // 1を2ビット左にシフト -> 0100 (0x04)
// //                            // 必要に応じてさらに追加
// //                            // EVENT_NEW_FEATURE = (1 << 3), // 1を3ビット左にシフト -> 1000 (0x08)
// //                            // EVENT_ANOTHER_ONE = (1 << 4)  // 0x10
// //} EventBits;                // enumに分かりやすい型名をつける (EventBits)

// // --- イベント構造体 ---
// typedef struct
// {
//   unsigned int ID; // イベントの識別子（この例ではSYS.EVENTと同じ意味）
//   void *pPARAM;    // イベントに付随するパラメータへのポインタ
// } Event;

// // --- システムの状態管理 ---
// typedef struct
// {
//   MainState MAIN;
//   SubState SUB;
//   unsigned int EVENT; // Detect_Eventが設定するイベントビット
// } SystemStatus;

// SystemStatus SYS;    // システム状態
// Event current_event; // 現在処理中のイベントデータ

// // --- 処理関数（void* 引数を受け取るように変更） ---
// void handle_A_X_Greet(void *param)
// {
//   if (param)
//   {
//     printf("[State A, Sub X] Handling GREET event. Parameter: '%s'\n", (char *)param);
//   }
//   else
//   {
//     printf("[State A, Sub X] Handling GREET event (no specific parameter).\n");
//   }
// }

// void handle_A_X_Goodbye(void *param)
// {
//   if (param)
//   {
//     // param が int* へのポインタであることを想定
//     printf("[State A, Sub X] Handling GOODBYE event. Reason code: %d\n", *(int *)param);
//     free(param); // mallocで確保したメモリはここで解放する責任がある
//   }
//   else
//   {
//     printf("[State A, Sub X] Handling GOODBYE event (no specific parameter).\n");
//   }
// }

// void handle_A_Y_Greet(void *param)
// {
//   if (param)
//   {
//     printf("[State A, Sub Y] Handling GREET event. Parameter: '%s' (from AY).\n", (char *)param);
//   }
//   else
//   {
//     printf("[State A, Sub Y] Handling GREET event (no specific parameter).\n");
//   }
// }

// void handle_B_X_Goodbye(void *param)
// {
//   if (param)
//   {
//     printf("[State B, Sub X] Handling GOODBYE event. Reason code: %d (from BX).\n", *(int *)param);
//     free(param); // mallocで確保したメモリはここで解放
//   }
//   else
//   {
//     printf("[State B, Sub X] Handling GOODBYY event (no specific parameter).\n");
//   }
// }

// void handle_unknown_event(void *param)
// { // param はここでは無視されることが多いが、型は合わせる
//   printf("Unknown event or no specific handler for current state/sub-state.\n");
//   printf("Current State: Main=%d, Sub=%d, Event=0x%02X\n", SYS.MAIN, SYS.SUB, SYS.EVENT);
//   // もし param が動的に確保されたものであれば、ここで解放を検討
//   // 例: if (SYS.EVENT == EVENT_UNKNOWN && param != NULL) { free(param); }
// }

// // --- イベントハンドラのエントリ構造体（引数 void* を受け取るように変更） ---
// typedef struct
// {
//   unsigned int event_bit;
//   void (*handler_function)(void *); // 引数として void* を受け取るように変更
// } EventDispatcherEntry;

// void do_nothing(void *param)
// { // 引数を受け取るように変更
//   // printf("No specific action for this state/sub-state/event combination.\n");
// }

// // --- ディスパッチテーブルの定義 ---
// EventDispatcherEntry handlers_A_X[] = {
//     {EVENT_GREET, handle_A_X_Greet},
//     {EVENT_GOODBYE, handle_A_X_Goodbye},
//     {0, NULL}};

// EventDispatcherEntry handlers_A_Y[] = {
//     {EVENT_GREET, handle_A_Y_Greet},
//     {0, NULL}};

// EventDispatcherEntry handlers_B_X[] = {
//     {EVENT_GOODBYE, handle_B_X_Goodbye},
//     {0, NULL}};

// EventDispatcherEntry *dispatch_table[NUM_MAIN_STATES][NUM_SUB_STATES] = {
//     {
//         handlers_A_X, // MAIN_STATE_A, SUB_STATE_X
//         handlers_A_Y  // MAIN_STATE_A, SUB_STATE_Y
//     },
//     {
//         handlers_B_X, // MAIN_STATE_B, SUB_STATE_X
//         NULL          // MAIN_STATE_B, SUB_STATE_Y (特定のハンドラリストなし)
//     }};

// // --- 関数: Clear_Event_Bit ---
// void Clear_Event_Bit(unsigned int bit_to_clear)
// {
//   SYS.EVENT &= ~bit_to_clear;
//   printf("Cleared event bit: 0x%02X. Current SYS.EVENT: 0x%02X\n", bit_to_clear, SYS.EVENT);
// }

// // --- 関数: Detect_Event ---
// void Detect_Event()
// {
//   char key_input;
//   printf("\nEnter 'g' (Greet), 'b' (Goodbye), 's' (Switch State), 'q' (Quit) or other for unknown: ");
//   key_input = getchar();
//   while (getchar() != '\n')
//     ; // バッファクリア

//   SYS.EVENT = EVENT_NONE;      // イベントビットをクリア
//   current_event.pPARAM = NULL; // パラメータポインタもクリア

//   switch (key_input)
//   {
//   case 'g':
//   {
//     SYS.EVENT |= EVENT_GREET;
//     // 挨拶する相手の名前をパラメータとして渡す
//     current_event.ID = SYS.EVENT;           // Event.IDをSYS.EVENTと同じにする
//     current_event.pPARAM = (void *)"World"; // 静的な文字列リテラルのポインタ
//     printf("Event detected: GREET (0x%02X) with param '%s'\n", SYS.EVENT, (char *)current_event.pPARAM);
//     break;
//   }
//   case 'b':
//   {
//     SYS.EVENT |= EVENT_GOODBYE;
//     // 別れの理由コードを数値として渡す（ヒープに確保）
//     current_event.ID = SYS.EVENT;
//     int *reason_code = malloc(sizeof(int));
//     if (reason_code == NULL)
//     {
//       perror("Failed to allocate memory for reason_code");
//       // エラー処理
//       return;
//     }
//     *reason_code = 101; // 例: 理由コード101
//     current_event.pPARAM = (void *)reason_code;
//     printf("Event detected: GOODBYE (0x%02X) with param %d\n", SYS.EVENT, *(int *)current_event.pPARAM);
//     break;
//   }
//   case 's':
//     SYS.MAIN = (SYS.MAIN == MAIN_STATE_A) ? MAIN_STATE_B : MAIN_STATE_A;
//     SYS.SUB = (SYS.SUB == SUB_STATE_X) ? SUB_STATE_Y : SUB_STATE_X;
//     printf("State Switched! New State: Main=%d, Sub=%d\n", SYS.MAIN, SYS.SUB);
//     break;
//   case 'q': // 終了コマンド
//     printf("Quitting program...\n");
//     exit(0); // プログラムを終了
//   default:
//     SYS.EVENT |= EVENT_UNKNOWN;
//     current_event.ID = SYS.EVENT;
//     printf("Event detected: UNKNOWN (0x%02X)\n", SYS.EVENT);
//     break;
//   }
// }

// // --- 関数: Process_Events ---
// void Process_Events()
// {
//   EventDispatcherEntry *current_handlers = NULL;
//   if (SYS.MAIN < NUM_MAIN_STATES && SYS.SUB < NUM_SUB_STATES)
//   {
//     current_handlers = dispatch_table[SYS.MAIN][SYS.SUB];
//   }

//   void (*function_to_call)(void *) = NULL; // 引数を持つ関数ポインタ型
//   unsigned int processed_bit = EVENT_NONE;

//   if (current_handlers != NULL)
//   {
//     for (int i = 0; current_handlers[i].handler_function != NULL; i++)
//     {
//       if ((SYS.EVENT & current_handlers[i].event_bit) == current_handlers[i].event_bit)
//       {
//         function_to_call = current_handlers[i].handler_function;
//         processed_bit = current_handlers[i].event_bit;
//         break;
//       }
//     }
//   }

//   if (function_to_call == NULL && SYS.EVENT != EVENT_NONE)
//   {
//     function_to_call = handle_unknown_event;
//     processed_bit = SYS.EVENT; // UNKNOWNイベントの場合もクリア対象に
//   }
//   else if (SYS.EVENT == EVENT_NONE)
//   {
//     return; // イベントなしの場合は何もせず終了
//   }

//   if (function_to_call != NULL)
//   {
//     function_to_call(current_event.pPARAM); // ここで Event.pPARAM を渡す！
//     Clear_Event_Bit(processed_bit);
//   }
// }

// // --- main関数 (メインループ) ---
// int main()
// {
//   SYS.MAIN = MAIN_STATE_A;
//   SYS.SUB = SUB_STATE_X;
//   SYS.EVENT = EVENT_NONE;

//   printf("--- Event Driven System Simulation with Event Parameters ---\n");
//   printf("Initial State: Main=%d (A), Sub=%d (X)\n", SYS.MAIN, SYS.SUB);
//   printf("Commands: 'g' (Greet), 'b' (Goodbye), 's' (Switch State), 'q' (Quit)\n");

//   while (1)
//   {
//     Detect_Event();
//     Process_Events();
//   }

//   return 0;
// }