#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here


bool equalPaths(Node * root)
{

    //base cases:
    if (root == nullptr){
        return true;
    }


    if (equalPaths(root->left) == true && equalPaths(root->right) == true){
        if (degreeCal(root->left) != degreeCal(root->right) && (degreeCal(root->left) > 1 || degreeCal(root->right) > 1))
        {
            return false;
        }
        return true;
    }
    else{
        return false;
    }
    
}

int degreeCal(Node * x)
{
    if (x == nullptr){
        return 0;
    }

    if (degreeCal(x->left) >= degreeCal(x->right)){
        return degreeCal(x->left)+1;
    }
    else{
        return degreeCal(x->right)+1;
    }
}


