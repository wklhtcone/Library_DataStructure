/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+
			Name:		Books.cpp
			Author:		Zhang Yun
			Version:	alpha 0.1
			Intro:		everything related to 
						the Books 
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+*/

#include <iostream>
#include <fstream>
#include "Books.h"
#include "Function.h"
#include "Global_define.h"
using namespace std;


//--------------------------------------------------------------------
//Class Book


// 默认构造函数
Book::Book() {
	Num_Borrowed = Num_Subscribe = Num_Available = Num_Sum = 0;

	Book_Limit = 0;
}

// 拷贝构造函数
Book::Book(const Book & book1) {
	Num_Available = book1.Num_Available;
	Num_Borrowed = book1.Num_Borrowed;
	Num_Subscribe = book1.Num_Subscribe;
	Num_Sum = book1.Num_Sum;

	Book_Author = book1.Book_Author;
	Book_Limit = book1.Book_Limit;
	Book_Name = book1.Book_Name;
	Book_Publisher = book1.Book_Publisher;
	Book_Type = book1.Book_Type;
	
	ID_pre = book1.ID_pre;
}

/* 自定义构造函数 
从 filename.txt 文件中读取初始化本类书信息
filename 是本书的ID前缀（8位）
例如 Book("00000001"); */
Book::Book(const string & filename) {
	ifstream file("book\\"+filename+".txt");

	string temp;

	getline(file, ID_pre);
	getline(file, Book_Name);
	getline(file, Book_Author);
	getline(file, Book_Publisher);
	getline(file, Book_Type);
	file >> Book_Limit;
	file >> Num_Sum;
	file >> Num_Borrowed;
	file >> Num_Subscribe;
	file >> Num_Available;

	file.close();
}

/*如果新购入的书不是已有书 
那么应该创建一本新的类型的书 
ID_pre 和 Number 自动生成 并建立三个文件日志*/
Book::Book(const string & bookname, const string & bookauthor, const string & bookpublisher, const string & booktype, const int & booklimit) {
	Num_Borrowed = Num_Subscribe =  0;
	Num_Available = Num_Sum = 1;
	// get the id and add this id
	string str = FileLine_Getline("logbook\\sys.txt", 5 + booklimit), name="0";
	long num = Convert_strtolong(str);
	num += 1; 
	str = Convert_longtostr(num);
	FileLine_Change("logbook\\sys.txt", 5 + booklimit, str);

	// creat the filename id
	name[0] = name[0] + booklimit;
	if (str.length() < 7)
		for (int i = 1; i <= 7 - str.length(); i += 1)
			name += "0";
	name += str;

	// change the booklist.txt
	string t = FileLine_Getline("book\\booklist.txt", 1);
	num = Convert_strtolong(t);
	num += 1;
	t = Convert_longtostr(num);
	FileLine_Change("book\\booklist.txt", 1, t);
	FileLine_Insert("book\\booklist.txt", num, name);

	// creat the book file
	fstream f1, f2, f3;
	f1.open("book\\"+name+".txt"); 
	f2.open("book\\" + name+"_books.txt"); 
	f3.open("book\\" +name+ "_subc.txt");

	f1 << name << endl;
	f1 << bookname << endl;
	f1 << bookauthor << endl;
	f1 << bookpublisher << endl;
	f1 << booktype << endl;
	f1 << booklimit << endl;
	f1 << 1 << endl;
	f1 << 0 << endl;
	f1 << 0 << endl;
	f1 << 1 << endl;
	f1.close(); 
	
	name += "0001";// the "str name" now is the complete id
	f2 << name << endl;
	t = Convert_longtostr(Day);
	f2 << t << endl;
	f2 << "000000000000" << endl;
	f2 << 0 << endl;
	f2 << 100 << endl;
	f2.close(); 
	
	f3.close();
}

// 默认析构函数
Book::~Book() {
	// nothing to do
}

// 根据 书名 作者 出版社 来判断 新输入的书是否与本类书等价
bool Book::is_equal(const string & name, const string & author, const string & publishor) {
	if (name == Book_Name && author == Book_Author && publishor == Book_Publisher)
		return true;
	return false;
}

// 购入了一本新的书 要求更新 
// <成员变量 索引文件 书表文件 预约文件> 
// 预约的人可以借书时 更新人的提示信息
void Book::book_addbook() {
	// add the total num in booklist
	Num_Sum += 1;
	long num = Convert_strtolong(FileLine_Getline("book\\" + ID_pre + ".txt", 7));
	num += 1;
	string str = Convert_longtostr(num);
	FileLine_Change("book\\" + ID_pre + ".txt", 7, str);

	// add in the ID_book.txt
		// get the total id
	string name = ID_pre, t;
	if (str.length() < 4)
		for (int i = 1; i <= 4 - str.length(); i += 1)
			name += "0";
	name += str;
	// write file
	FileLine_Insert("book\\" + ID_pre + "_books.txt", (num - 1) * 5 + 0, name);
	FileLine_Insert("book\\" + ID_pre + "_books.txt", (num - 1) * 5 + 1, Convert_longtostr(Day));
	// sovle the subcribe people
	str = FileLine_Getline("book\\" + ID_pre + "_subc.txt", 1);
	if (str != "") {
		FileLine_Delete("book\\" + ID_pre + "_subc.txt", 1);
		long count = 0;
		fstream f("people\\" + str + "_subc.txt");
		while (f.peek() != EOF) {
			count += 1;
			getline(f, t);
			if (t == ID_pre)
				break;
		}
		f.close();
		FileLine_Change("people\\" + str + "_subc.txt", count, name);
		FileLine_Change("people\\" + str + "_subc.txt", count + 1, "1");
		FileLine_Insert("book\\" + ID_pre + "_books.txt", (num - 1) * 5 + 2, str);
		FileLine_Insert("book\\" + ID_pre + "_books.txt", (num - 1) * 5 + 3, Convert_longtostr(2));
		// add the subc num
		FileLine_Change("book\\" + ID_pre + ".txt", 9, Convert_longtostr(1 + Convert_strtolong(FileLine_Getline("book\\" + ID_pre + ".txt", 9))));
		Num_Subscribe += 1;
	}
	else {
		FileLine_Insert("book\\" + ID_pre + "_books.txt", (num - 1) * 5 + 2, "000000000000");
		FileLine_Insert("book\\" + ID_pre + "_books.txt", (num - 1) * 5 + 3, Convert_longtostr(0));
		// add the subc num
		FileLine_Change("book\\" + ID_pre + ".txt", 10, Convert_longtostr(1 + Convert_strtolong(FileLine_Getline("book\\" + ID_pre + ".txt", 10))));
		Num_Available += 1;
	}
	FileLine_Insert("book\\" + ID_pre + "_books.txt", (num - 1) * 5 + 4, Convert_longtostr(100));

}

// 增加了一个预约者 (此时应该已经判断出来应该是要预约 因为已经没书可借)
// 要求更新 <成员变量 索引文件 书表文件 预约文件>
void Book::book_subscribe(const string & ID_borrower) {
	// update the ID_subc.txt
	FileLine_Insert("book\\" + ID_pre + "_subc.txt", 0, ID_borrower);
	// update the file about people
	FileLine_Change("people\\" + ID_borrower + ".txt", 9, Convert_longtostr(1 + Convert_strtolong(FileLine_Getline("people\\"+ID_borrower + ".txt", 9))));
	FileLine_Insert("people\\" + ID_borrower + "_subc.txt", 0, ID_pre);
	FileLine_Insert("people\\" + ID_borrower + "_subc.txt", 1, "0");

}

// 将预约转换成借阅 (此时这个人应该不在预约队列中了 已经有书被预约给他了)
// 要求更新 <成员变量 索引文件 书表文件 预约文件>
void Book::book_convert(const string & ID_book, const string & ID_borrower) {
	// we get the complete id from the ID_Borrower
	
	/*fstream f("people\\" + ID_borrower + "_subc.txt");
	string str;
	long count = 0;
	while (f.peek() != EOF) {
		count += 1;
		getline(f, str);
		if (ID_pre == str.substr(0, 8))
			break;
	}
	f.close();*/
	long count = FileLine_Getnumber("people\\" + ID_borrower + "_subc.txt", ID_book);

	// delete the subc info in the people's subc file
	FileLine_Delete("people\\" + ID_borrower + "_subc.txt", count);
	FileLine_Delete("people\\" + ID_borrower + "_subc.txt", count);
	// add the borrow info in the people book file and change the people.txt
	FileLine_Change("people\\" + ID_borrower + ".txt", 8, Convert_longtostr(1 + Convert_strtolong(FileLine_Getline("people\\" + ID_borrower + ".txt", 8))));
	FileLine_Change("people\\" + ID_borrower + ".txt", 9, Convert_longtostr(-1 + Convert_strtolong(FileLine_Getline("people\\" + ID_borrower + ".txt", 9))));
	
	FileEnd_Add("people\\" + ID_borrower + "_books.txt", ID_book);
	FileEnd_Add("people\\" + ID_borrower + "_books.txt", "30");
	
	// update the booksfile
	long lnum = FileLine_Getnumber("book\\" + ID_pre + "_books.txt", ID_book);
	FileLine_Change("book\\" + ID_pre + "_books.txt", lnum + 2, ID_borrower);
	FileLine_Change("book\\" + ID_pre + "_books.txt", lnum + 3, "1");
	FileLine_Change("book\\" + ID_pre + "_books.txt", lnum + 4, "30");
	
	FileLine_Change("book\\" + ID_pre + ".txt", 8, Convert_longtostr(1 + Convert_strtolong(FileLine_Getline("book\\" + ID_pre + ".txt", 8))));
	FileLine_Change("book\\" + ID_pre + ".txt", 9, Convert_longtostr(-1 + Convert_strtolong(FileLine_Getline("book\\" + ID_pre + ".txt", 9))));

	// update the member var
	Num_Borrowed += 1;
	Num_Subscribe -= 1;
}

// 增加了一个借阅者 
// 要求更新 <成员变量 索引文件 书表文件 预约文件>
void Book::book_borrow(const string & ID_borrower) {
	// update the member var and change the ID.txt
	Num_Borrowed += 1;
	Num_Available -= 1;
	FileLine_Change("book\\" + ID_pre + ".txt", 8, Convert_longtostr(Num_Borrowed));

	// find one avaliable book and remember the linenumber of it
	fstream F("book\\" + ID_pre + "_books.txt");
	string str;

	long linenumber = 0;
	while (F.peek() != EOF) {
		linenumber += 1;
		for (int i = 1; i <= 4; i += 1)
			getline(F, str);
		if (0 == Convert_strtolong(str))
			break;
		else
			getline(F, str);
	}
	F.close();

	// update the ID_Books.txt
	FileLine_Change("book\\" + ID_pre + "books.txt", linenumber - 1, ID_borrower);
	FileLine_Change("book\\" + ID_pre + "books.txt", linenumber , "1");
	FileLine_Change("book\\" + ID_pre + "books.txt", linenumber + 1, "30");

	// update the file about people
	FileLine_Change("people\\" + ID_borrower + ".txt", 8, Convert_longtostr(1 + Convert_strtolong(FileLine_Getline("people\\" + ID_borrower + ".txt", 8))));
	FileEnd_Add("people\\" + ID_borrower + "_books.txt", FileLine_Getline("people\\" + ID_pre + "books.txt", linenumber - 3));
	FileEnd_Add("people\\" + ID_borrower + "_books.txt", "30");
}

// 还书动作相关操作 
// 要求更新 <成员变量 索引文件 书表文件 预约文件> 
// 预约的人可以借书时 更新人的提示信息
void Book::book_return(const string & ID_book, const string & ID_borrower) {
	// we need to return the book an get the subc people for the book
	// so we judge it first 
		// get the subc people ID in str
	string str = FileLine_Getline("book\\" + ID_pre + "_subc.txt", 1);
	if ("" == str) {//we have at least one subc people
		FileLine_Delete("book\\" + ID_pre + "_subc.txt", 1);
		// file about book
		long num = FileLine_Getnumber("book\\" + ID_pre + "_books.txt", ID_book);
		FileLine_Change("book\\" + ID_pre + "_books.txt", num + 2, str);
		FileLine_Change("book\\" + ID_pre + "_books.txt", num + 3, "2");
		FileLine_Change("book\\" + ID_pre + "_books.txt", num + 4, "100");
		
		Num_Borrowed -= 1;
		Num_Subscribe += 1;

		FileLine_Change("book\\" + ID_pre + ".txt", 8, Convert_longtostr(-1 + Convert_strtolong(FileLine_Getline("book\\" + ID_pre + ".txt", 8))));
		FileLine_Change("book\\" + ID_pre + ".txt", 9, Convert_longtostr(1 + Convert_strtolong(FileLine_Getline("book\\" + ID_pre + ".txt", 9))));
		// file about people subc
		num = FileLine_Getnumber("people\\" + str + "_subc.txt", ID_pre);
		FileLine_Change("people\\" + str + "_subc.txt", num, ID_book);
		FileLine_Change("people\\" + str + "_subc.txt", num + 1, "1");
	}
	else {// we don't have to solve the subc because there is no subc people
		long num = FileLine_Getnumber("book\\" + ID_pre + "_books.txt", ID_book);
		FileLine_Change("book\\" + ID_pre + "_books.txt", num + 2, "000000000000");
		FileLine_Change("book\\" + ID_pre + "_books.txt", num + 3, "0");
		FileLine_Change("book\\" + ID_pre + "_books.txt", num + 4, "100");

		Num_Borrowed -= 1;
		Num_Available += 1;

		FileLine_Change("book\\" + ID_pre + ".txt", 8, Convert_longtostr(-1 + Convert_strtolong(FileLine_Getline("book\\" + ID_pre + ".txt", 8))));
		FileLine_Change("book\\" + ID_pre + ".txt", 10, Convert_longtostr(1 + Convert_strtolong(FileLine_Getline("book\\" + ID_pre + ".txt", 10))));
	}
	// solve the money
	long line = FileLine_Getnumber("people\\" + ID_borrower + "_books.txt", ID_book);
	FileLine_Delete("people\\" + ID_borrower + "_books.txt", line);
	long dueday = Convert_strtolong(FileLine_Getline("people\\" + ID_borrower + "_books.txt", line));
	FileLine_Delete("people\\" + ID_borrower + "_books.txt", line);

	if(dueday<0){
		FileLine_Change("people\\" + ID_borrower + ".txt", 6, Convert_longtostr(dueday + Convert_strtolong(FileLine_Getline("people\\" + ID_borrower + ".txt", 6))));
		FileLine_Change("people\\" + ID_borrower + ".txt", 7, Convert_doubletostr(0.1*(-dueday)));
	}
	FileLine_Change("people\\" + ID_borrower + ".txt", 8, Convert_longtostr(-1 + Convert_strtolong(FileLine_Getline("people\\" + ID_borrower + ".txt", 8))));

}

// 打印本书信息
void Book::book_print() {
	cout << endl;

	cout << "ID前缀：" << ID_pre << endl;
	cout << "书名：" << Book_Name << endl;
	cout << "作者：" << Book_Author << endl;
	cout << "出版社：" << Book_Publisher << endl;
	cout << "类别：" << Book_Type << endl;
	cout << "可用数量：" << Num_Available << endl;
	
	cout << endl;

}

string Book::get_bookname() {
	return Book_Name;
}

string Book::get_bookauthor() {
	return Book_Author;
}

string Book::get_bookpublisher() {
	return Book_Publisher;
}

string Book::get_booktype() {
	return Book_Type;
}

string Book::get_ID_pre() {
	return ID_pre;
}

int Book::get_bookLimit() {
	return Book_Limit;
}

//-------------------------------------------------------------------
// class ManageBooks

ManageBooks::ManageBooks() {
	Num_Sum_Of_Books = 0;
}

ManageBooks::ManageBooks(const ManageBooks & M) {
	Num_Sum_Of_Books = M.Num_Sum_Of_Books;
}

ManageBooks::ManageBooks(const string & filename) {
	Num_Sum_Of_Books = Convert_strtolong(FileLine_Getline("book\\" + filename + ".txt",1));
}

ManageBooks::~ManageBooks() {
}

// 购入图书 读入新书信息 
// 并要求判断 是修改现有文件还是建立新文件 
// booklist文件也应该被更新
void ManageBooks::BookList_Buy(const string & bookname, const string & bookauthor, const string & bookpublisher, const string & booktype, const int & booklimit) {
	fstream f("book\\booklist.txt");

	string str;
	while (f.peek() != EOF) {
		getline(f, str);
		Book B("book\\" + str + ".txt");
		if (B.is_equal(bookname, bookauthor, bookpublisher)) {
			B.book_addbook();
			f.close();
			return;
		}
	}
	f.close();

	Book B(bookname, bookauthor, bookpublisher, booktype, booklimit);
}

// 借书 读入两个id信息 
// 并且自动判断 进行借书动作还是预约动作
// (当 Subscribe 为 TRUE 时才进行预约 否则不进行预约) 
// 借书时候自动分配完整id书
void ManageBooks::BookList_Borrow(const string & id_pre, const string & id_person, bool Subscribe) {
	long num = Convert_strtolong(FileLine_Getline("book\\" + id_pre + ".txt", 10));
	Book B("book\\" + id_pre + ".txt");

	if (num >= 1) {// we can lent it
		B.book_borrow(id_person);
	}
	else {// we don't have avabliable book
		if (Subscribe)
			B.book_subscribe(id_person);
	}
}

// 根据id信息完成还书操作
void ManageBooks::BookList_Return(const string & id_book, const string & id_person) {
	string id_pre = id_book;
	id_pre = id_pre.substr(0, 8);

	Book B("book\\" + id_pre + ".txt");
	B.book_return(id_book, id_person);
}

// 根据id信息完成预约转借书操作
void ManageBooks::BookList_Convert(const string & id_book, const string & id_person) {
	Book B("book\\" + id_book + ".txt");

	B.book_convert(id_book, id_person);
}

// 本函数内部有 有限状态自动机 完成对各个项目的搜索操作
void ManageBooks::BookList_Find(const int limit) {
	cout << endl;
	cout << "以下是查书选项:\n";
	cout << "  1:根据ID\n";
	cout << "  2:根据书名\n";
	cout << "  3:根据作者\n";
	cout << "  4:根据出版社\n";
	cout << "  5:根据类别\n";
	cout << "选择: ";
	string str, t;
	getline(cin, str);

	while (!(0 == str.length() && ('1' <= str[0] && str[0] <= '5'))) {
		cout << "输入错误\n重新输入: ";
		getline(cin, str);
	}
	char c = str[0];
	cout << "\n 输入: ";
	getline(cin, str);

	fstream f("book\\booklist.txt");

	switch (c) {
	case '1'://id
		while (f.peek() != EOF) {
			getline(f, t);
			if (string::npos != t.find(str)) {
				Book B("book\\" + t + ".txt");
				B.book_print();
				break;
			}
		}
		break;
	case '2'://bookname
		while (f.peek() != EOF) {
			getline(f, t);
			Book B("book\\" + t + ".txt");

			if (string::npos != B.get_bookname().find(str)) {
				B.book_print();
			}
		}
		break;
	case '3'://author
		while (f.peek() != EOF) {
			getline(f, t);
			Book B("book\\" + t + ".txt");

			if (string::npos != B.get_bookauthor().find(str)) {
				B.book_print();
			}
		}
		break;
	case '4'://publisher
		while (f.peek() != EOF) {
			getline(f, t);
			Book B("book\\" + t + ".txt");

			if (string::npos != B.get_bookpublisher().find(str)) {
				B.book_print();
			}
		}
		break;
	case '5'://type
		while (f.peek() != EOF) {
			getline(f, t);
			Book B("book\\" + t + ".txt");

			if (string::npos != B.get_booktype().find(str)) {
				B.book_print();
			}
		}
		break;
	}
	f.close();
}

// 每新的一天 刷新所有图书的借阅信息
void ManageBooks::BookList_DateFlash() {
	fstream f_list("book\\booklist.txt"),f_book,f_t;
	string str, line;

	getline(f_list, str);
	while (f_list.peek() != EOF) {
		getline(f_list, str);

		// flash a book
			// write in temp
		f_book.open("book\\" + str + "_books.txt");
		f_t.open("logbook\\temp.txt", ios::trunc);
		int count = 0;
		while (f_book.peek() != EOF) {
			count += 1;
			getline(f_book, line);
			if (count != 5)
				f_t << line << endl;
			else {
				count = 0;
				long num = -1 + Convert_strtolong(line);
				f_t << Convert_longtostr(num) << endl;
			}
		}
		f_book.close();
		f_t.close();
			
			//rewtite in file
		f_book.open("book\\" + str + "_books.txt", ios::trunc);
		f_t.open("logbook\\temp.txt");

		while (f_t.peek() != EOF) {
			getline(f_t, line);
			f_book << line << endl;
		}

		f_book.close();
		f_t.close();
	}

	f_list.close();
}

// 打印所有书信息
void ManageBooks::BookList_Print() {
	fstream f("book\\booklist.txt");
	string str;

	getline(f, str);
	while (f.peek() != EOF) {
		getline(f, str);
		Book B("book\\" + str + ".txt");
		B.book_print;
	}

	f.close();
}
