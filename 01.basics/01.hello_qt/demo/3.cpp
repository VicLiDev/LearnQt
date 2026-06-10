void test(){
    WordEngine *w=new WordEngine();
    w->Open("e://b.doc",false);
//    w->replaceText("a","test");
    w->replacePic("a","D:/q.jpg");
 
    QStringList list;
    list.append("1");
    list.append("2");
    list.append("3");
    list.append("4");
    QStringList list2;
    list2.append("5");
    list2.append("6");
    list2.append("7");
    list2.append("8");
 
 
 
    QList<QStringList> l1;
    l1.append(list);
    l1.append(list2);
 
 
    w->insertTable("b",4,l1);
 
    w->save("e://a.doc");
    w->close();
}