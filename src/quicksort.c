//
//  quicksort.c
//  PointerTest
//
//  Created by Carola Walter on 03.02.14.
//  Copyright (c) 2014 Carola Walter. All rights reserved.
//


#include <stdio.h>

void quicksortup (int *a, int n) {
    int i, j, p, t;
    
    if (n < 2)
        return;
    p = a[n / 2];
    for (i = 0, j = n - 1;; i++, j--) {
        
        while (a[i] < p)
            i++;
        while (a[j] > p)
            j--;
        
        if (i >= j)
            break;
        t = a[i];
        a[i] = a[j];
        a[j] = t;
    }
    quicksortup(a, i);
    quicksortup(a + i, n - i);
}

void quicksortdown (int *a, int n) {
    int i, j, p, t;
    
    if (n < 2)
        return;
    p = a[n / 2];
    for (i = 0, j = n - 1;; i++, j--) {
        
        while (a[i] > p)
            i++;
        while (a[j] < p)
            j--;
        
        if (i >= j)
            break;
        t = a[i];
        a[i] = a[j];
        a[j] = t;
    }
    quicksortdown(a, i);
    quicksortdown(a + i, n - i);
}


int main(int argc, const char * argv[])
{
    
    // insert code here...
    printf("Hello, World!\n");
    
    int a[] = {4, 65, 2, -31, 0, 99, 2, 83, 782, 1, 19, 22, 45,3,4,66,12,0};
    int n = sizeof a / sizeof a[0];
    int i;
    for (i = 0; i < n; i++)
        printf("%d%s", a[i], i == n - 1 ? "\n" : " ");
    quicksortup(a, n);
    for (i = 0; i < n; i++)
        printf("%d%s", a[i], i == n - 1 ? "\n" : " ");
    
    int b[] = {4, 65, 2, -31, 0, 99, 2, 83, 782, 1, 19, 22, 45,3,4,66,12,0};
    for (i = 0; i < n; i++)
        printf("%d%s", b[i], i == n - 1 ? "\n" : " ");
    int s = sizeof b / sizeof b[0];
    quicksortdown(b, s);
    for (i = 0; i < s; i++)
        printf("%d%s", b[i], i == n - 1 ? "\n" : " ");
    return 0;
}


