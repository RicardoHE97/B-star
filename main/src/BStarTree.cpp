#include "../header/BStarTree.h"
#include <stack>

 BStarTree::~BStarTree()
 {
    Node* currentNode;
    std::queue<Node*> nodeQueue;

    nodeQueue.push(root);

    while (!nodeQueue.empty()) {
        currentNode = nodeQueue.front();
        nodeQueue.pop();

        delete currentNode;
    }
 }

bool BStarTree::add(double val)
{
    bool added;
    Node* nodeAdd = nullptr;    //Node where it will add the number if the number
                                //doesn't exist in the tree.
    Node* currentNode;

    nodeAdd = findPlace(val);
    if (nodeAdd != nullptr) {
        nodeAdd->addItem(val);
        currentNode = nodeAdd;
        while (currentNode != nullptr && currentNode->isOverloaded()) {
                handleOverload(currentNode);
                currentNode = currentNode->getAncestor();
        }


        added = true;
    }else{
        added = false;
    }

    return added;
}

void BStarTree::handleOverload(Node* overloadedNode)
{
    if (!overloadedNode->IsRoot()) {
        if (!this->searchSpace(overloadedNode)) {
            if (!this->isLeftmost(overloadedNode)) {
                splitLeft(overloadedNode);    //This split every node with their left sibling
            }else{
                splitRight(overloadedNode); //This split the leftmost node with his right sibling
                                            //because he has no left sibling
            }
        }
    }else{
        splitRoot();
    }
}

bool BStarTree::erase(double val)
{
    bool erased;
    Node* nodeErase = nullptr;  //Node where it will add the number if the number
                                //doesn't exist in the tree.
    Node* currentNode;
    //do findPlaceerase
    nodeErase = findPlaceErase(val);
    if (nodeErase == nullptr) {
        return false;
    }
    nodeErase->keys().remove(val);

    currentNode = nodeErase;

    if (!nodeErase->isLeaf()) {
        currentNode = getGreaterMinor(nodeErase, val);
        nodeErase->keys().push_back( currentNode->keys().back() );
        nodeErase->keys().sort();
        currentNode->keys().pop_back();
    }

    Node* ancestor;
    while (currentNode != nullptr && currentNode->isUnderloaded()) {
        ancestor = currentNode->getAncestor();
        handleUnderload(currentNode);
        currentNode = ancestor;
    }

    erased = true;

    return erased;
}

//Remind to delete some sibling if we need to delete the current node that is underloaded
void BStarTree::handleUnderload(Node* underloadedNode)
{
    if (!underloadedNode->IsRoot()) {
        if (!this->searchSpaceErase(underloadedNode)) {
            if(this->isLeftmost(underloadedNode)){
                mergeLeft(underloadedNode);
            }else if(this->isRightmost(underloadedNode)){
                mergeRight(underloadedNode);
            }else{ //not leftmost nor rightmost
                merge(underloadedNode);
            }
        }
    }else{
        mergeRoot();
    }
}

bool BStarTree::find(double val)
{
    //remember to check if there is a more optimal way to do this
    return findPlace(val) != nullptr ? true : false;
}

//this search can be optimized because when searching a node there is no need to keep
//searching for a value once the values of the node are bigger than the searched value.
Node* BStarTree::findPlace(double val)
{
    Node* currentNode = root;
    std::list<Node*>::iterator child;


    while(!currentNode->children().empty()){
        child = currentNode->children().begin();
        /*for(auto key = currentNode->keys().begin();
                key != currentNode->keys().end();
                ++key){*/
        for(auto key : currentNode->keys()){
            if(key == val){ //exceptional case, the value already is in the tree
                return nullptr;
            }else if(key < val){
                ++child;
            }
        }
        if(!currentNode->children().empty()){
            currentNode = *child;
        }
    }

    auto firstKey = currentNode->keys().begin();
    auto endKey = currentNode->keys().end();


    //search if the value to add is in the leaf node about to be returned
    if (std::find(firstKey, endKey, val) != endKey) {
        return nullptr;
    }

    return currentNode;
}

// can probably be more optimized
Node* BStarTree::findPlaceErase(double val)
{
    Node* currentNode = root;
    std::list<Node*>::iterator child;

    while(!currentNode->children().empty()){
        child = currentNode->children().begin();
        /*for(auto key = currentNode->keys().begin();
                key != currentNode->keys().end();
                ++key){*/
        for(auto key : currentNode->keys()){
            if(key == val){ //this is what we want when erasing
                return currentNode;
            }else if(key < val){
                ++child;
            }
        }
        if(!currentNode->children().empty()){
            currentNode = *child;
        }
    }

    auto firstKey = currentNode->keys().begin();
    auto endKey = currentNode->keys().end();

    //search if the value to add is in the leaf node. If it is, then it is returned,
    //if not, then it wasn't found in the tree.
    if (std::find(firstKey, endKey, val) != endKey) {
        return currentNode;
    }

    return nullptr; //the value wasn't found so nullptr signals this
}

bool BStarTree::searchSpace(Node* node)
{
    Node* nodeCopy;
    bool foundSpace;

    foundSpace = true;
    nodeCopy = node;

    if (this->areLeftSiblingsFull(nodeCopy)){
        if (this->areRightSiblingsFull(nodeCopy)) {
            foundSpace = false;
        }else{
            this->rotateRight(nodeCopy);
        }
    }else{
        this->rotateLeft(nodeCopy);
    }

    return foundSpace;
}

bool BStarTree::searchSpaceErase(Node* node)
{
    Node* nodeCopy;
    bool foundSpace;

    foundSpace = true;
    nodeCopy = node;

    if (this->areLeftSiblingsAtMinimum(nodeCopy)){
        if (this->areRightSiblingsAtMinimum(nodeCopy)) {
            foundSpace = false;
        }else{
            this->rotateRightErase(nodeCopy);
        }
    }else{
        this->rotateLeftErase(nodeCopy);
    }

    return foundSpace;

}

bool BStarTree::areLeftSiblingsFull(Node* node) const
{
    Node* ancestor = node->getAncestor();
    bool nodeIsFull = true;

    //this checks all the nodes before the received one to see if at least one of them is
    //not full
    for(auto leftSibling = ancestor->children().begin(); *leftSibling != node;  ++leftSibling){
         if ( !(*leftSibling)->isFull() ) {
            nodeIsFull = false;
            break;
        }
    }

    return nodeIsFull;
}
bool BStarTree::areRightSiblingsFull(Node* node) const
{
    Node* ancestor = node->getAncestor();
    bool nodeIsFull = true;

    for(auto rightSibling = ancestor->children().rbegin(); *rightSibling != node; ++rightSibling){
         if ( !(*rightSibling)->isFull() ) {
            nodeIsFull = false;
            break;
        }
    }

    return nodeIsFull;
}

bool BStarTree::areLeftSiblingsAtMinimum(Node* node) const
{
    Node* ancestor = node->getAncestor();
    bool nodeIsAtMinimum = true;

    //this checks all the nodes before the received one to see if at least one of them is
    //not full
    for(auto leftSibling = ancestor->children().begin(); *leftSibling != node;  ++leftSibling){
         if ( !(*leftSibling)->isAtMinimum() ) {
            nodeIsAtMinimum = false;
            break;
        }
    }

   return nodeIsAtMinimum;
}
bool BStarTree::areRightSiblingsAtMinimum(Node* node) const
{
    Node* ancestor = node->getAncestor();
    bool nodeIsAtMinimum = true;

    for(auto rightSibling = ancestor->children().rbegin(); *rightSibling != node; ++rightSibling){
         if ( !(*rightSibling)->isAtMinimum() ) {
            nodeIsAtMinimum = false;
            break;
        }
    }

    return nodeIsAtMinimum;
}

bool BStarTree::isLeftmost(Node* node) const
{
    return *node->getAncestor()->children().begin() ==  node ? true : false;
}

bool BStarTree::isRightmost(Node* node) const
{
    return *node->getAncestor()->children().rbegin() ==  node ? true : false;
}
Node* BStarTree::getLeftSibling(Node* node)
{
    Node* ancestor, *sibling;

    ancestor = node->getAncestor();

    sibling = nullptr;

    auto it = ancestor->children().begin();
    while(*it != node){
        ++it;
    }

    if (it != ancestor->children().begin()) {
        sibling = *prev(it);
    }

    return sibling;
}

Node* BStarTree::getRightSibling(Node* node)
{
    Node* ancestor, *sibling;

    ancestor = node->getAncestor();

    sibling = nullptr;

    auto it = ancestor->children().begin();
    while(*it != node){
        ++it;
    }

    if (it != prev(ancestor->children().end())) {
        sibling = *next(it);
    }

    return sibling;
}

bool BStarTree::rotateLeft(Node* node)
{
    Node *currentNode, *ancestor, *leftSibling, *child;
    std::list<double>::iterator ancestorKey;
    std::list<Node*>::iterator nodeIt;

    currentNode = node;
    do {
        ancestor = currentNode->getAncestor();
        ancestorKey = ancestor->keys().begin();
        for(nodeIt = next(ancestor->children().begin()); *nodeIt != currentNode; ++nodeIt){
            ++ancestorKey;
        }
        leftSibling = *prev(nodeIt);

        //key rotation
        leftSibling->keys().push_back(*ancestorKey);
        *ancestorKey = currentNode->keys().front();
        currentNode->keys().pop_front();

        //child rotation
        if(!currentNode->children().empty()){
            child = currentNode->children().front();
            currentNode->children().pop_front();
            dynamic_cast<NormalNode*>(child)->setAncestor(leftSibling);
            leftSibling->children().push_back(child);
        }

        currentNode = leftSibling;
    } while(!isLeftmost(currentNode) && currentNode->isOverloaded());

     return true;
}

bool BStarTree::rotateRight(Node* node)
{
    Node *currentNode, *ancestor, *rightSibling, *child;
    std::list<double>::iterator ancestorKey;
    std::list<Node*>::iterator nodeIt;

    currentNode = node;
    do {
        ancestor = currentNode->getAncestor();
        ancestorKey = ancestor->keys().begin();
        for(nodeIt = ancestor->children().begin(); *nodeIt != currentNode; ++nodeIt){
            ++ancestorKey;
        }
        rightSibling = *next(nodeIt);

        //key rotation
        rightSibling->keys().push_front(*ancestorKey);
        *ancestorKey = currentNode->keys().back();
        currentNode->keys().pop_back();

        //child rotation
        if(!currentNode->children().empty()){
            child = currentNode->children().back();
            currentNode->children().pop_back();
            dynamic_cast<NormalNode*>(child)->setAncestor(rightSibling);
            rightSibling->children().push_front(child);
        }

        currentNode = rightSibling;
    } while(!isRightmost(currentNode) && currentNode->isOverloaded());

    return true;

}

bool BStarTree::rotateLeftErase(Node* node)
{
    Node *currentNode, *ancestor, *leftSibling, *child;
    std::list<double>::iterator ancestorKey;
    std::list<Node*>::iterator nodeIt;

    currentNode = node;
    do {
        ancestor = currentNode->getAncestor();
        ancestorKey = ancestor->keys().begin();
        for(nodeIt = next(ancestor->children().begin()); *nodeIt != currentNode; ++nodeIt){
            ++ancestorKey;
        }
        leftSibling = *prev(nodeIt);

        //key rotation
        currentNode->keys().push_front(*ancestorKey);
        *ancestorKey = leftSibling->keys().back();
        leftSibling->keys().pop_back();

        //child rotation
        if(!leftSibling->children().empty()){
            child = leftSibling->children().back();
            leftSibling->children().pop_back();
            dynamic_cast<NormalNode*>(child)->setAncestor(currentNode);
            currentNode->children().push_front(child);
        }

        currentNode = leftSibling;
    } while(!isLeftmost(currentNode) && currentNode->isUnderloaded());

    return true;
}

bool BStarTree::rotateRightErase(Node* node)
{
    Node *currentNode, *ancestor, *rightSibling, *child;
    std::list<double>::iterator ancestorKey;
    std::list<Node*>::iterator nodeIt;

    currentNode = node;
    do {
        ancestor = currentNode->getAncestor();
        ancestorKey = ancestor->keys().begin();
        for(nodeIt = ancestor->children().begin(); *nodeIt != currentNode; ++nodeIt){
            ++ancestorKey;
        }
        rightSibling = *next(nodeIt);

        //key rotation
        currentNode->keys().push_back(*ancestorKey);
        *ancestorKey = rightSibling->keys().front();
        rightSibling->keys().pop_front();

        //child rotation
        if(!rightSibling->children().empty()){
            child = rightSibling->children().front();
            rightSibling->children().pop_front();
            dynamic_cast<NormalNode*>(child)->setAncestor(currentNode);
            currentNode->children().push_back(child);
        }

        currentNode = rightSibling;
    } while(!isRightmost(currentNode) && currentNode->isUnderloaded());

    return true;
}

void BStarTree::splitRoot(){
    Node *child1, *child2;

    child1 = new NormalNode(mOrder, false, root, id++, 2);
    child2 = new NormalNode(mOrder, false, root, id++, 2);

    auto putKeys = [&](unsigned limit, Node*& lNode){
        for (std::size_t i = 0; i < limit; i++) {
            lNode->keys().push_back( root->keys().front() );
            root->keys().pop_front();
        }
    };

    unsigned limitRoot = std::floor((2*mOrder - 2)/3) /*root->keys().size()/2*/;
    putKeys(limitRoot, child1);

    double auxKey = root->keys().front();
    root->keys().pop_front();

    putKeys(limitRoot, child2);

    root->keys().push_front(auxKey);
    //////////////////////////////////////////s///////////////////
    //Review this lamnda
    ////////////////////////////////////////////
    auto putChildren = [&](unsigned limit, Node*& lNode){
        if(!root->children().empty()){
            for (std::size_t i = 0; i < limit; i++) {
                lNode->children().push_back( root->children().front() );
                dynamic_cast<NormalNode*>(lNode->children().back())->setAncestor(lNode);
                root->children().pop_front();
            }
        }

    };
    //////////////////////////////////////////////////
    unsigned limitForChild1 = child1->keys().size() + 1;
    unsigned limitForChild2 = child2->keys().size() + 1;


    putChildren(limitForChild1, child1);
    putChildren(limitForChild2, child2);


    root->children().push_back(child1);
    root->children().push_back(child2);
}


void BStarTree::splitLeft(Node* node)
{
    Node *leftSibling, *ancestor;
    double ancestorKeyCopy;
    std::list<double>::iterator ancestorKey;
    std::list<Node*>::iterator nodeIt;

    ancestor = node->getAncestor();
    ancestorKey = ancestor->keys().begin();
    for(nodeIt = next(ancestor->children().begin()); *nodeIt != node; ++nodeIt){
        ++ancestorKey;
    }

    ancestorKeyCopy = *ancestorKey;
    ancestor->keys().erase(ancestorKey);

    leftSibling = *prev(nodeIt);

    //moves all the keys of the left sibling to an auxiliar list, leaving the sibling empty
    std::list<double> auxList(std::move(leftSibling->keys()));
    auxList.push_back(ancestorKeyCopy);
    //moves all the keys of the node to the auxiliar list, leaving the node empty
    auxList.merge(node->keys());

    Node *newNode; //new node that goes in the middle of the current node and its left sibling
    newNode = new NormalNode(mOrder, false, ancestor, id++, ancestor->getHeight() + 1);

    ancestor->children().push_back(newNode);

    auto putKeys = [&auxList](unsigned limit, Node*& lNode){
        for (std::size_t i = 0; i < limit; i++) {
            lNode->keys().push_back( auxList.front() );
            auxList.pop_front();
        }
    };

    auto putKeyAncestor = [&ancestor, &auxList](){
        ancestor->keys().push_back( auxList.front() );
        auxList.pop_front();
    };

    //accommodate keys in the nodes
    unsigned limitOne = std::floor( (2*mOrder - 2)/3 );
    putKeys(limitOne, leftSibling);

    putKeyAncestor();

    unsigned limitTwo = std::floor( (2*mOrder - 1)/3 );
    putKeys(limitTwo, newNode);

    putKeyAncestor();

    unsigned limitThree = std::floor( 2*mOrder/3 );
    putKeys(limitThree, node);

    ancestor->keys().sort();
    ancestor->children().sort( compareKeyNodes );

    //accommodate children in the nodes.
    std::list<Node*> auxListChildren(std::move(leftSibling->children()));
    auxListChildren.merge(node->children());
    auxListChildren.sort(compareKeyNodes);

    auto putChildren = [&auxListChildren](unsigned limit, Node*& lNode){
        if (!auxListChildren.empty()) {
            for (std::size_t i = 0; i < limit; i++) {
                lNode->children().push_back( auxListChildren.front() );
                auxListChildren.pop_front();
                dynamic_cast<NormalNode*>(lNode->children().back())->setAncestor(lNode);
            }
        }
    };

    putChildren(limitOne+1, leftSibling);
    putChildren(limitTwo+1, newNode);
    putChildren(limitThree+1, node);


    //now it is necessary to check if the ancestor is overloaded and handle that
    //THIS IS RECURSIVE, REMEMBER TO CHANGE IT
    /*while(ancestor != nullptr && ancestor->isOverloaded()){
        handleOverload(ancestor);
        ancestor = ancestor->getAncestor();
    }*/
    //RECURSIVE, ALERT DANGER
}

/*
this split with the right sibling.
*/
void BStarTree::splitRight(Node* node)
{
    Node *rightSibling, *ancestor;
    double ancestorKeyCopy;
    std::list<double>::iterator ancestorKey;
    std::list<Node*>::iterator nodeIt;

    ancestor = node->getAncestor();
    ancestorKey = ancestor->keys().begin();
    for(nodeIt = ancestor->children().begin(); *nodeIt != node; ++nodeIt){
        ++ancestorKey;
    }

    ancestorKeyCopy = *ancestorKey;
    ancestor->keys().erase(ancestorKey);

    rightSibling = *next(nodeIt);

    //moves all the keys of the node to the auxiliar list, leaving the node empty
    std::list<double> auxList(std::move(node->keys()));
    auxList.push_back(ancestorKeyCopy);
    //moves all the keys of the right sibling to an auxiliar list, leaving the sibling empty
    auxList.merge(rightSibling->keys());

    Node *newNode; //new node that goes in the middle of the current node and its right sibling
    newNode = new NormalNode(mOrder, false, ancestor, id++, ancestor->getHeight() + 1);

    ancestor->children().push_back(newNode);

    auto putKeys = [&auxList](unsigned limit, Node*& lNode){
        for (std::size_t i = 0; i < limit; i++) {
            lNode->keys().push_back( auxList.front() );
            auxList.pop_front();
        }
    };

    auto putKeyAncestor = [&ancestor, &auxList](){
        ancestor->keys().push_back( auxList.front() );
        auxList.pop_front();
    };

    unsigned limitOne = std::floor( (2*mOrder - 2)/3 );
    putKeys(limitOne, node);
    putKeyAncestor();

    unsigned limitTwo = std::floor( (2*mOrder - 1)/3 );
    putKeys(limitTwo, newNode);
    putKeyAncestor();

    unsigned limitThree = std::floor( 2*mOrder/3 );
    putKeys(limitThree, rightSibling);

    ancestor->keys().sort();
    ancestor->children().sort( compareKeyNodes );


    //accommodate children in the nodes.
    std::list<Node*> auxListChildren(std::move(node->children()));
    auxListChildren.merge(rightSibling->children());
    auxListChildren.sort(compareKeyNodes);

    auto putChildren = [&auxListChildren](unsigned limit, Node*& lNode){
        if (!auxListChildren.empty()) {
            for (std::size_t i = 0; i < limit; i++) {
                lNode->children().push_back( auxListChildren.front() );
                auxListChildren.pop_front();
                dynamic_cast<NormalNode*>(lNode->children().back())->setAncestor(lNode);
            }
        }
    };

    putChildren(limitOne+1, node);
    putChildren(limitTwo+1, newNode);
    putChildren(limitThree+1, rightSibling);

    //now it is necessary to check if the ancestor is overloaded and handle that
    //THIS IS RECURSIVE, REMEMBER TO CHANGE IT
    /*while(ancestor != nullptr && ancestor->isOverloaded()){
        handleOverload(ancestor);
        ancestor = ancestor->getAncestor();
    }*/
    //RECURSIVE, ALERT DANGER
}

void BStarTree::mergeRoot()
{

}

//Case where there are left and right siblings.
//this can be used when mergin left and right
void BStarTree::merge(Node* node)
{
    Node *ancestor, *leftSibling, *rightSibling;
    std::list<double>::iterator ancestorKey;
    std::list<Node*>::iterator nodeIt;

    ancestor = node->getAncestor();
    ancestorKey = ancestor->keys().begin();
    for(nodeIt = next(ancestor->children().begin()); *nodeIt != node; ++nodeIt){
        ++ancestorKey;
    }

    ancestor->keys().erase(ancestorKey);

    leftSibling = *prev(nodeIt);
    rightSibling = *next(nodeIt);

    std::list<double> auxList( std::move(leftSibling->keys()) );
    auxList.push_back(*ancestorKey);
    ancestor->keys().erase(ancestorKey);
    auxList.merge(node->keys());
    auxList.push_back(*++ancestorKey);
    ancestor->keys().erase(ancestorKey);
    auxList.merge(rightSibling->keys());

    auto putKeys = [&auxList](unsigned limit, Node*& lNode){
        for (std::size_t i = 0; i < limit; i++) {
            lNode->keys().push_back( auxList.front() );
            auxList.pop_front();
        }
    };

    unsigned limitOne = auxList.size() / 2;
    unsigned limitTwo = limitOne;
    if(auxList.size() % 2 == 0){
        limitOne -= 1;
    }

    putKeys(limitOne, leftSibling);
    ancestor->keys().push_back( auxList.front() );
    auxList.pop_front();
    putKeys(limitTwo, node);

    //move all childrens before removing the right sibling
    std::list<Node*> auxListChildren( std::move(leftSibling->children()) );
    auxListChildren.merge(node->children());
    auxListChildren.merge(rightSibling->children());

    auto putChildren = [&auxListChildren](unsigned limit, Node*& lNode){
        for (std::size_t i = 0; i < limit; i++) {
            lNode->children().push_back( auxListChildren.front() );
            auxListChildren.pop_front();
        }
    };

    putChildren(limitOne+1, leftSibling);
    putChildren(limitTwo+1, node);

    ancestor->children().remove(rightSibling);
}

void BStarTree::mergeLeft(Node* node)
{

}

void BStarTree::mergeRight(Node* node)
{

}

Node* BStarTree::getGreaterMinor(Node *node, double val)
{
    if (node->isLeaf()) {
        return nullptr;
    }

    std::list<double>::iterator ancestorKey;
    std::list<Node*>::iterator childIt;

    childIt = node->children().begin();

    for (ancestorKey = node->keys().begin(); *ancestorKey < val; ancestorKey++) {
        childIt++;
    }

    Node* greaterMinor;
    greaterMinor = *childIt;
    while (!greaterMinor->isLeaf()) {
        greaterMinor = greaterMinor->children().back();
    }

    return greaterMinor;

}

void BStarTree::print()
{
    Node* currentNode;
    std::queue<Node*> nodeQueue;
    unsigned oldHeight = 0;

    nodeQueue.push(root);

    while (!nodeQueue.empty()) {
        currentNode = nodeQueue.front();
        nodeQueue.pop();

            if(currentNode->getHeight() > oldHeight){
                std::cout << "Printing the nodes with height: " << ++oldHeight << std::endl;
            }

        std::cout << "Node's id, height and number of children: " << std::endl;
        std::cout << currentNode->getId() << " | " << currentNode->getHeight() << " | ";
        std::cout << currentNode->children().size() << std::endl;
        std::cout << "Id of the acestor: " ;
        if(currentNode != root){
            std::cout << currentNode->getAncestor()->getId();
        }
        std::cout << std::endl;
        currentNode->print();

        for(Node* child : currentNode->children()){
            nodeQueue.push(child);
        }
    }
}

unsigned BStarTree::addFromFile(std::string filepath)
{
    std::ifstream file;
    double number;
    unsigned addedCount = 0;

    file.open(filepath);

    if(!file.is_open()){
        std::cerr << "couldn't open the file with path: " << filepath << std::endl;
        return 0;
    }

    while(file >> number){
        if(add(number)){ //checks if it adds an element
            ++addedCount;
        }
    }

    file.close();

    return addedCount;
}

unsigned BStarTree::eraseFromFile(std::string filepath)
{
    std::ifstream file;
    double number;
    unsigned erasedCount = 0;

    file.open(filepath);

    if(!file.is_open()){
        std::cerr << "couldn't open the file with path: " << filepath << std::endl;
        return 0;
    }

    while(file >> number){
        if(erase(number)){ //checks if it erases an element
            ++erasedCount;
        }
    }

    file.close();

    return erasedCount;
}

void BStarTree::generateFile(int size)
{
    std::ofstream oFile;

    oFile.open("files/file.txt");
    if(!oFile.is_open()){
        std::cerr << "Couldn't read file with path " << "files/file.txt" << std::endl;
    }

    for(int i = 0; i < size; ++i){
        oFile << rand() % 2000;
    }

    oFile.close();
}

void BStarTree::testAddAndDelete(std::string filepath, int elementsToLeave)
{
    std::ifstream iaddFile;
    double number;

    std::vector<double> elements(30);
    iaddFile.open(filepath);
    if(!iaddFile.is_open()){
        std::cerr << "Couldn't read file with path: " << filepath << std::endl;
        return;
    }

    while(iaddFile >> number){
        elements.push_back(number);
        add(number);
    }

    iaddFile.close();

    std::random_shuffle(elements.begin(), elements.end()); //shuffles the vector
    int elementsToErase = elements.size() - elementsToLeave;
    for(int i = 0; i < elementsToErase; ++i){
        erase(elements[i]);
    }
}

bool compareKeyNodes(Node* nodeA, Node* nodeB)
{
    return *nodeA->keys().begin() < *nodeB->keys().begin();
}
