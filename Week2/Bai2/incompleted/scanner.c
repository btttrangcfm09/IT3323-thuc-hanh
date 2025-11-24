/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/**
 * @brief Bỏ qua các ký tự trắng (space, tab, newline) liên tiếp.
 *
 * Hàm này sẽ liên tục đọc ký tự tiếp theo từ luồng vào cho đến khi
 * gặp một ký tự không phải là ký tự trắng hoặc kết thúc file (EOF).
 */
void skipBlank() {
  // Lặp trong khi ký tự hiện tại vẫn là ký tự trắng và chưa hết file
  while (currentChar != EOF && charCodes[currentChar] == CHAR_SPACE) {
    readChar();
  }
}

/**
 * @brief Bỏ qua một khối comment.
 *
 * Comment trong ngôn ngữ KPL bắt đầu bằng `(*` và kết thúc bằng `*)`.
 * Hàm này sẽ đọc và bỏ qua tất cả các ký tự cho đến khi gặp chuỗi `*)`.
 * Nếu gặp cuối file (EOF) trước khi comment kết thúc, hàm sẽ báo lỗi.
 */
void skipComment() {
  while (1) {
    // Đọc ký tự tiếp theo
    readChar();

    // Nếu gặp cuối file, comment không được đóng lại -> Lỗi
    if (currentChar == EOF) {
      error(ERR_ENDOFCOMMENT, lineNo, colNo);
      return; // Kết thúc hàm để tránh vòng lặp vô hạn
    }
    // Nếu gặp ký tự '*'
    else if (charCodes[currentChar] == CHAR_TIMES) {
      // Đọc tiếp để kiểm tra xem có phải là ')' không
      readChar();

      // Vẫn cần kiểm tra EOF ngay sau khi đọc
      if (currentChar == EOF) {
        error(ERR_ENDOFCOMMENT, lineNo, colNo);
        return;
      }
      // Nếu ký tự tiếp theo là ')', chúng ta đã tìm thấy kết thúc comment
      else if (charCodes[currentChar] == CHAR_RPAR) {
        // Đọc ký tự tiếp theo để con trỏ không còn ở ')' nữa
        readChar();
        return; // Thoát khỏi hàm
      }
      // Nếu không phải '*)', tiếp tục vòng lặp
    }
  }
}

/**
 * @brief Bỏ qua comment dạng dòng (line comment).
 *
 * Comment dạng dòng bắt đầu bằng `//` và kết thúc tại cuối dòng hoặc EOF.
 * Hàm này sẽ đọc và bỏ qua tất cả các ký tự cho đến khi gặp ký tự xuống dòng hoặc EOF.
 */
void skipLineComment() {
  // Đọc cho đến khi gặp ký tự xuống dòng hoặc EOF
  while (currentChar != EOF && currentChar != '\n') {
    readChar();
  }
  // Nếu gặp ký tự xuống dòng, đọc tiếp để di chuyển sang dòng mới
  if (currentChar == '\n') {
    readChar();
  }
}

/**
 * @brief Đọc một định danh (identifier) hoặc một từ khoá (keyword).
 *
 * Một định danh bắt đầu bằng một chữ cái, theo sau là các chữ cái hoặc chữ số.
 * Hàm này đọc chuỗi ký tự hợp lệ, sau đó kiểm tra xem nó có phải là một
 * từ khoá đã định nghĩa trước hay không.
 *
 * @return Con trỏ tới Token đã được tạo.
 */
Token* readIdentKeyword(void) {
  Token *token = makeToken(TK_IDENT, lineNo, colNo);
  int length = 0;

  // Lặp trong khi ký tự hiện tại là chữ cái hoặc chữ số
  while (currentChar != EOF && (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT)) {
    // Ghi ký tự vào chuỗi của token
    // Thêm kiểm tra để tránh tràn bộ đệm (buffer overflow)
    if (length < MAX_IDENT_LEN) {
      token->string[length] = (char)currentChar;
    }
    length++;
    readChar();
  }
  
  // Kết thúc chuỗi
  token->string[length < MAX_IDENT_LEN ? length : MAX_IDENT_LEN] = '\0';

  // Kiểm tra nếu định danh quá dài
  if (length > MAX_IDENT_LEN) {
    error(ERR_IDENTTOOLONG, token->lineNo, token->colNo);
    // Vẫn trả về token (dù bị lỗi) để bộ phân tích có thể tiếp tục
    return token;
  }
  
  // Kiểm tra xem chuỗi có phải là một từ khoá không
  TokenType keywordType = checkKeyword(token->string);

  // Nếu là từ khoá, cập nhật lại loại token
  if (keywordType != TK_NONE) {
    token->tokenType = keywordType;
  }

  return token;
}

/**
 * @brief Đọc một hằng số (number literal).
 *
 * Hàm này đọc một chuỗi các chữ số liên tiếp và chuyển đổi chúng thành
 * một giá trị số nguyên.
 *
 * @return Con trỏ tới Token đã được tạo.
 */
Token* readNumber(void) {
  Token* token = makeToken(TK_NUMBER, lineNo, colNo);
  int length = 0;

  // Lặp trong khi ký tự hiện tại là chữ số
  while (currentChar != EOF && charCodes[currentChar] == CHAR_DIGIT) {
    if (length < MAX_IDENT_LEN) { // Tái sử dụng MAX_IDENT_LEN cho giới hạn độ dài số
       token->string[length] = (char)currentChar;
    }
    length++;
    readChar();
  }

  // Kết thúc chuỗi
  token->string[length] = '\0';
  
  // Chuyển chuỗi số thành giá trị kiểu integer
  // Lưu ý: atoi không có cơ chế kiểm tra lỗi tràn số.
  // Trong một trình biên dịch thực tế, nên dùng strtol để kiểm tra.
  token->value = atoi(token->string);

  return token;
}

/**
 * @brief Đọc một hằng ký tự (character literal).
 *
 * Hằng ký tự hợp lệ trong KPL có dạng 'c' (một ký tự duy nhất trong cặp nháy đơn).
 * Hàm này xử lý việc đọc và xác thực cấu trúc này.
 *
 * @return Con trỏ tới Token đã được tạo.
 */
Token* readConstChar(void) {
  Token *token = makeToken(TK_CHAR, lineNo, colNo);
  
  // Hàm này được gọi khi ký tự hiện tại là dấu nháy đơn đầu tiên.
  // Ta cần đọc ký tự tiếp theo để lấy nội dung của hằng ký tự.
  readChar();

  if (currentChar == EOF) {
    // Lỗi: Hằng ký tự chưa được đóng và đã hết file.
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }
    
  // Lấy ký tự bên trong cặp nháy
  token->string[0] = currentChar;
  token->string[1] = '\0'; // Kết thúc chuỗi

  // Đọc ký tự tiếp theo, mong đợi đó là dấu nháy đơn đóng
  readChar();

  if (currentChar != EOF && charCodes[currentChar] == CHAR_SINGLEQUOTE) {
    // Hợp lệ, đọc ký tự tiếp theo để di chuyển con trỏ ra khỏi hằng ký tự
    readChar();
    return token;
  } else {
    // Lỗi: Thiếu dấu nháy đơn đóng hoặc có quá nhiều ký tự.
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }
}

/**
 * @brief Đọc một chuỗi ký tự (string literal).
 *
 * Chuỗi ký tự hợp lệ trong KPL có dạng "text" (chuỗi ký tự trong cặp nháy kép).
 * Hàm này xử lý việc đọc và xác thực cấu trúc này.
 *
 * @return Con trỏ tới Token đã được tạo.
 */
Token* readString(void) {
  Token *token = makeToken(TK_STRING, lineNo, colNo);
  int length = 0;
  
  // Hàm này được gọi khi ký tự hiện tại là dấu nháy kép đầu tiên.
  // Ta cần đọc ký tự tiếp theo để lấy nội dung của chuỗi.
  readChar();

  // Đọc cho đến khi gặp dấu nháy kép đóng hoặc EOF
  while (currentChar != EOF && charCodes[currentChar] != CHAR_DOUBLEQUOTE) {
    if (length < MAX_IDENT_LEN) {
      token->string[length] = (char)currentChar;
    }
    length++;
    readChar();
  }

  // Kết thúc chuỗi
  token->string[length < MAX_IDENT_LEN ? length : MAX_IDENT_LEN] = '\0';

  if (currentChar == EOF) {
    // Lỗi: Chuỗi chưa được đóng và đã hết file.
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }

  // Đọc ký tự tiếp theo để di chuyển con trỏ ra khỏi dấu nháy kép đóng
  readChar();
  return token;
}

/**
 * @brief Lấy token tiếp theo từ luồng vào.
 *
 * Đây là hàm trung tâm của bộ phân tích từ vựng (scanner). Nó đọc ký tự
 * hiện tại và quyết định loại token tương ứng, sau đó gọi hàm xử lý phù hợp.
 *
 * @return Con trỏ tới Token tiếp theo.
 */
Token* getToken(void) {
  Token *token;

  // Nếu đã hết file, trả về token EOF
  if (currentChar == EOF) {
    return makeToken(TK_EOF, lineNo, colNo);
  }

  switch (charCodes[currentChar]) {
  // Bỏ qua ký tự trắng và comment
  case CHAR_SPACE:
    skipBlank();
    return getToken();

  // Nhận dạng định danh và từ khóa
  case CHAR_LETTER:
    return readIdentKeyword();

  // Nhận dạng hằng số
  case CHAR_DIGIT:
    return readNumber();
  
  // Nhận dạng hằng ký tự
  case CHAR_SINGLEQUOTE:
    return readConstChar();

  // Nhận dạng chuỗi ký tự
  case CHAR_DOUBLEQUOTE:
    return readString();

  // Các toán tử và dấu câu (Symbols)
  case CHAR_PLUS:
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar();
    return token;
  case CHAR_MINUS:
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar();
    return token;
  case CHAR_TIMES:
    token = makeToken(SB_TIMES, lineNo, colNo);
    readChar();
    // Kiểm tra xem có phải là toán tử ** không
    if (currentChar != EOF && charCodes[currentChar] == CHAR_TIMES) {
      token->tokenType = SB_EXP; // Đây là toán tử **
      readChar();
    }
    return token;
  case CHAR_SLASH:
    token = makeToken(SB_SLASH, lineNo, colNo);
    readChar();
    // Kiểm tra xem có phải là comment dạng // không
    if (currentChar != EOF && charCodes[currentChar] == CHAR_SLASH) {
      free(token); // Giải phóng token đã tạo vì đây là comment
      skipLineComment();
      return getToken(); // Bắt đầu lại việc lấy token mới
    }
    return token;
  case CHAR_PERCENT:
    token = makeToken(SB_MOD, lineNo, colNo);
    readChar();
    return token;
  case CHAR_EQ:
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar();
    return token;
  case CHAR_COMMA:
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar();
    return token;

  // Các toán tử có thể có 2 ký tự (>=, <=, !=, :=, (. , .), (* *)
  case CHAR_GT:
    token = makeToken(SB_GT, lineNo, colNo);
    readChar(); // Đọc ký tự tiếp theo
    if (currentChar != EOF && charCodes[currentChar] == CHAR_EQ) {
      token->tokenType = SB_GE; // Đây là toán tử >=
      readChar();
    }
    return token;
  case CHAR_LT:
    token = makeToken(SB_LT, lineNo, colNo);
    readChar();
    if (currentChar != EOF && charCodes[currentChar] == CHAR_EQ) {
      token->tokenType = SB_LE; // Đây là toán tử <=
      readChar();
    }
    return token;
  case CHAR_EXCLAIMATION:
    token = makeToken(TK_NONE, lineNo, colNo); // Tạm thời tạo token NONE
    readChar();
    if (currentChar != EOF && charCodes[currentChar] == CHAR_EQ) {
      token->tokenType = SB_NEQ; // Đây là toán tử !=
      readChar();
      return token;
    } else {
      // Nếu '!' đứng một mình thì là ký hiệu không hợp lệ
      error(ERR_INVALIDSYMBOL, token->lineNo, token->colNo);
      return token; // Trả về token lỗi
    }
  case CHAR_COLON:
    token = makeToken(SB_COLON, lineNo, colNo);
    readChar();
    if (currentChar != EOF && charCodes[currentChar] == CHAR_EQ) {
      token->tokenType = SB_ASSIGN; // Đây là phép gán :=
      readChar();
    }
    return token;
  case CHAR_PERIOD:
    token = makeToken(SB_PERIOD, lineNo, colNo);
    readChar();
    if (currentChar != EOF && charCodes[currentChar] == CHAR_RPAR) {
      token->tokenType = SB_RSEL; // Đây là dấu truy cập mảng .)
      readChar();
    }
    return token;
  case CHAR_LPAR:
    token = makeToken(SB_LPAR, lineNo, colNo);
    readChar();
    if (currentChar == EOF) return token;

    if (charCodes[currentChar] == CHAR_PERIOD) {
      token->tokenType = SB_LSEL; // Đây là dấu truy cập mảng (.
      readChar();
    } else if (charCodes[currentChar] == CHAR_TIMES) {
      free(token); // Giải phóng token đã tạo vì đây là comment
      skipComment();
      return getToken(); // Bắt đầu lại việc lấy token mới
    }
    return token; // Nếu không phải trường hợp đặc biệt, nó chỉ là (

  case CHAR_RPAR:
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar();
    return token;
  
  // Ký tự không hợp lệ
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar(); 
    return token;
  }
}

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%d)\n", token->value); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_STRING: printf("TK_STRING(\"%s\")\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_STRING: printf("KW_STRING\n"); break;
  case KW_BYTES: printf("KW_BYTES\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;
  case KW_REPEAT: printf("KW_REPEAT\n"); break;
  case KW_UNTIL: printf("KW_UNTIL\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_MOD: printf("SB_MOD\n"); break;
  case SB_EXP: printf("SB_EXP\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }
    
  return 0;
}
