
#include <iostream>
#include <unordered_set>
#include <set>
#include <iterator>
#include <vector>
#include <algorithm>
#include <fstream>
#include <math.h>
#include "cpu_bench.hpp"

using namespace std;

//Бинарный поиск с возвратом индекса на итераторах с возвратом индекса

int binary_search_find_index(vector<int>::iterator beg, vector<int>::iterator en, int data) {
    vector<int>::iterator it = std::lower_bound(beg, en, data);
    if (it == en || *it != data) {
        return -1;
    } else {
        std::size_t index = std::distance(beg, it);
        return index;
    }   
}

//Бинарный поиск с возвратом индекса на итераторах с возвратом итератора

vector<int>::iterator binary_search_iterated(vector<int>* arr, vector<int>::iterator low, vector<int>::iterator high, int x){
    if (high >= low){
        vector<int>::iterator mid = low + (high - low)/2;

        if (*mid == x){
            return mid;
        }
        else{
            if (*mid > x){
                return binary_search_iterated(arr, low, mid - 1, x);
            }
            else{
                return binary_search_iterated(arr, mid + 1, high, x);
            }
        }

    }
    else{
        return low;
    }

}


/*
The most obvious alg
Comparing all the elements in A and B with each other 
and inserting them in C
Complexity is O(|A|*|B|)
*/
void simple_intersection(unordered_set<int>* A, unordered_set<int>* B, unordered_set<int>* C){ 
    unordered_set<int>::const_iterator it_A, it_B;
    unordered_set<int>::const_iterator it_C;
    it_A = A->begin();
    it_B = B->begin();
    while (it_A != A->end()){
        while (it_B != B->end()){
            //cout << *it_B << ' ' << *it_A <<endl;
            if(*it_A == *it_B){
                C->insert(*it_A);
            }
            ++it_B;
        }
        it_B = B->begin();
        ++it_A;
    }




}

//TODO: сделать обертку
//B не должен быть мощнее A
void BaezaYates_step(vector<int>* A, vector<int>* B, vector<int>::iterator A_begin, vector<int>::iterator A_end, vector<int>::iterator B_begin, vector<int>::iterator B_end, unordered_set<int> *C){
    if (A_end - A_begin <= 0 || B_end - B_begin <= 0){
        return;
    }
    vector<int>::iterator midB = B_begin + (B_end - B_begin)/2;
    int midBval = *midB;
    vector<int>::iterator midA = binary_search_iterated(A, A_begin, A_end, midBval);
    vector<int>::iterator a_res,b_res;
    if ((midA-A_begin) > (midB-B_begin)){
        BaezaYates_step(A,B,A_begin, midA, B_begin, midB, C);
    }
    else{
        BaezaYates_step(B,A,B_begin, midB, A_begin, midA, C);
    }
    if (*midA == midBval){
        C->insert(midBval);
    }
    if ((A_end - midA) > (B_end - midB)){
        BaezaYates_step(A,B,midA, A_end, midB+1, B_end, C);
    }
    else{
        BaezaYates_step(B,A,midB+1, B_end, midA, A_end, C);
    }
    
}
//Пока работает только при отсутствии сортировки, B не должен быть мощнее A
//TODO: написать оболочку-симметризатор

void GallopingIntersection(unordered_set<int>* A, unordered_set<int>* B, unordered_set<int>* C, bool isSorted = false){
    int low = 0;
    int diff, high;
    int k;

    if (!isSorted){
        vector<int> A_sorted(A->begin(), A->end());
        vector<int> B_sorted(B->begin(), B->end()); //creating unsorted duplicates and sort them as vectors
        std::sort(A_sorted.begin(), A_sorted.end());
        std::sort(B_sorted.begin(), B_sorted.end());



        for(int i=0; i<B_sorted.size(); ++i){  //< or <= I dunno
            //cout << "B[i]:" << B_sorted[i] << "   ";
            diff = 1;
            while (low + diff <= A_sorted.size() && A_sorted[low + diff] <= B_sorted[i]){
                diff *= 2;
            }
            high = min(int(A_sorted.size()), low + diff);
            k = binary_search_find_index((A_sorted.begin() + low), (A_sorted.begin() + high), B_sorted[i]);
            if(k != -1){
                C->insert(B_sorted[i]);
                //cout << B_sorted[i];
            }
            low = k;
            //cout << endl;
            
        }


    }



} 

int main(){
    //рабочие сеты
    unordered_set<int> b;
    unordered_set<int> a;

    //сеты результатов
    unordered_set<int> c;
    unordered_set<int> e;

    double startTimeBaeza, endTimeBaeza; //бенчмаркинг BY
    double startTimeGallop, endTimeGallop; // бенчмаркинг Gallop


    int itA, itB;

    //Получение сетов из файлов
    std::ifstream fh1("set1.txt");
    while(fh1>>itA){
        a.insert(itA);
    }
    fh1.close();

    std::ifstream fh2("set2.txt");
    while(fh2>>itB){
        b.insert(itB);
    }
    fh2.close();
    //выводим размеры
    cout<<"Collected size A: " << a.size() << endl;
    cout<<"Collected size B: " << b.size() <<endl;


    
    vector<int> aV(a.begin(), a.end());
    vector<int> bV(b.begin(), b.end());
    
    startTimeBaeza = getCPUTime();

    sort(aV.begin(), aV.end());
    sort(bV.begin(), bV.end());
    BaezaYates_step(&aV, &bV, aV.begin(),aV.end(),bV.begin(),bV.end(),&e);

    endTimeBaeza = getCPUTime();


    startTimeGallop = getCPUTime();

    GallopingIntersection(&a,&b,&c);

    endTimeGallop = getCPUTime();
    


    //выводим количество элементов в пересечениях и время работы
    cout << "a and b intersection Gallop: " << c.size() << endl;
    fprintf( stderr, "CPU time used for Gallop = %lf\n", (endTimeGallop - startTimeGallop) );

    cout << "a and b intersection BY: ";
    cout << e.size() << endl;
    fprintf( stderr, "CPU time used for BY= %lf\n", (endTimeBaeza - startTimeBaeza) );
    
    return 0;
}
