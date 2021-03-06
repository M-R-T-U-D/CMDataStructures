#pragma once

#include <iostream>
#include <tuple>
#include "BinarySearchTreeNode.h"

template<typename T>
class BinarySearchTree
{
public:
	BinarySearchTree()
		:
		root(nullptr)
	{
	}
	
	//BinarySearchTree(BinarySearchTreeNode<T>* const root)
	//	:
	//	root(root)
	//{
	//}

	BinarySearchTree(const T& data)
		:
		root(new BinarySearchTreeNode<T>(data))
	{
	}

	// Delete constructors which may cause headache and bugs
	BinarySearchTree(const BinarySearchTree<T>&) = delete;
	BinarySearchTree(BinarySearchTree<T>&&) = delete;

	~BinarySearchTree()
	{
		cleanUpTree(root);
	}

	void printTree(BinarySearchTreeNode<T>* node = nullptr)
	{
		std::cout << "Printing the BST\n";
		std::cout << "|-- = left node (value < parent value)\n";
		std::cout << "\\-- = right/root node (value > parent value)\n\n";
		if (node == nullptr)
		{
			if (root != nullptr)
				printTree("", root, false);
		}
		else
		{
			printTree("", node, false);
		}
	}

	inline 
	std::tuple<BinarySearchTreeNode<T>*, BinarySearchTreeNode<T>*> 
	findInorderSuccessor(BinarySearchTreeNode<T>* currNode)
	{
		if (currNode->hasRight())
		{
			return findInorderSuccessorHelper(currNode, currNode->getRight());
		}
		return std::make_tuple(nullptr, nullptr);
	}

	void removeNode(const T& data, BinarySearchTreeNode<T>* parentNode, BinarySearchTreeNode<T>* currNode)
	{
		if (currNode == nullptr)
		{
			return;
		}

		if (data < currNode->getData())
		{
			return removeNode(data, currNode, currNode->getLeft());
		} 
		else if (data > currNode->getData())
		{
			return removeNode(data, currNode, currNode->getRight());
		}
		else
		{
			// Current node contains the given data.
			// There are 4 options for deletion:
			//	1: currNode has no children -> let the parent point to nullptr and then delete the currNode
			//	2: currNode has only left child -> make left child the new currNode and delete the old currNode
			//	3: currNode has only right child -> make the parent left/right ref point to right child the currNode and delete the old currNode
			//	4: currNode has both children:
			//		4.1: if the direct right node of currNode does not have a left child, then:
			//			4.1.1: make the right node the new currNode
			//			4.1.2: make the child of new currNode the left child of old currNode
			//			4.1.3: make parent node point to new currNode
			//			4.1.4: delete old currNode
			//		4.2: if the direct right node of currNode does have a left child, then search for inorder successor node by traversing to the deepest left node
			//		4.3: make successor the new root of the tree
			//			4.3.1: if successor has right child, make the left child of parent of the successor point to that child
			if (parentNode == nullptr) // root contains the value we need to remove
			{
				if (!currNode->hasLeft() && !currNode->hasRight())
				{
					delete root;
					root = nullptr;
				}
				else if (!currNode->hasRight() && currNode->hasLeft())
				{
					root = currNode->getLeft();
					delete currNode;
				}
				else if (currNode->hasRight() && !currNode->hasLeft())
				{
					root = currNode->getRight();
					delete currNode;
				}
				else
				{
					const auto parentAndSuccessorNode = findInorderSuccessor(currNode);
					BinarySearchTreeNode<T>* parentSuccessor = std::get<0>(parentAndSuccessorNode);
					BinarySearchTreeNode<T>* nodeSuccessor = std::get<1>(parentAndSuccessorNode);

					if (nodeSuccessor == currNode->getRight()) // successor node is the direct right node from the root
					{
						nodeSuccessor->setLeft(root->getLeft());
						root = nodeSuccessor;
						delete currNode;
					}
					else // successor node is somewhere in the tree
					{
						parentSuccessor->setLeft(nodeSuccessor->getRight());
						nodeSuccessor->setLeft(root->getLeft());
						nodeSuccessor->setRight(root->getRight());
						root = nodeSuccessor;
						delete currNode;
					}
				}
			}
			else
			{
				if (!currNode->hasLeft() && !currNode->hasRight())
				{
					setChildFromParent(parentNode, currNode, nullptr);
					delete currNode;
				}
				else if (!currNode->hasRight() && currNode->hasLeft())
				{
					setChildFromParent(parentNode, currNode, currNode->getLeft());
					delete currNode;
				}
				else if (currNode->hasRight() && !currNode->hasLeft())
				{
					setChildFromParent(parentNode, currNode, currNode->getRight());
					delete currNode;
				}
				else
				{
					const auto parentAndSuccessorNode = findInorderSuccessor(currNode);
					BinarySearchTreeNode<T>* parentSuccessor = std::get<0>(parentAndSuccessorNode);
					BinarySearchTreeNode<T>* nodeSuccessor = std::get<1>(parentAndSuccessorNode);

					if (nodeSuccessor == currNode->getRight())
					{
						nodeSuccessor->setLeft(currNode->getLeft());
						setChildFromParent(parentNode, currNode, nodeSuccessor);
						delete currNode;
					}
					else
					{
						parentSuccessor->setLeft(nodeSuccessor->getRight());
						nodeSuccessor->setLeft(currNode->getLeft());
						nodeSuccessor->setRight(currNode->getRight());
						setChildFromParent(parentNode, currNode, nodeSuccessor);
						delete currNode;
					}
				}
			}
			
			currNode = nullptr;
		}
	}

	void removeNode(const T& data)
	{
		return removeNode(data, nullptr, root);
	}

	void insertNode(const T& data)
	{
		// if root is not pointing to a node yet, then just add the toAddNode as root.
		if (root == nullptr)
		{
			root = new BinarySearchTreeNode<T>(data);
		}
		else
		{
			BinarySearchTreeNode<T>* currNode = root;
			while (currNode != nullptr)
			{
				if (data < currNode->getData())
				{
					if (currNode->hasLeft())
					{
						currNode = currNode->getLeft();
					}
					else
					{
						currNode->setLeft(new BinarySearchTreeNode<T>(data));
						return;
					}						
				}
				else if (data > currNode->getData())
				{
					if (currNode->hasRight())
					{
						currNode = currNode->getRight();
					}
					else
					{
						currNode->setRight(new BinarySearchTreeNode<T>(data));
						return;
					}
				}
				else
				{
					// don't add a node with the same data value twice, just return
					return;
				}
			}
		}
	}

	BinarySearchTreeNode<T>* DFS(const T& data, BinarySearchTreeNode<T>* currRoot)
	{
		if (currRoot != nullptr)
		{
			if (currRoot->getData() == data)
				return currRoot;

			if (currRoot->hasLeft())
			{
				auto foundLeft = DFS(data, currRoot->getLeft());
				if (foundLeft)
					return foundLeft;
			}

			if (currRoot->getRight())
			{
				auto foundRight = DFS(data, currRoot->getRight());
				if (foundRight)
					return foundRight;
			}
		}

		return nullptr;
	}

	BinarySearchTreeNode<T>* DFS(const T& data)
	{
		return DFS(data, root);
	}

private:
	// from https://stackoverflow.com/questions/36802354/print-binary-tree-in-a-pretty-way-using-c
	void printTree(const std::string& prefix, BinarySearchTreeNode<T>* node, bool isLeft)
	{
		if (node != nullptr)
		{
			std::cout << prefix;

			std::cout << (isLeft ? "|-- " : "\\-- ");

			// print the value of the node
			std::cout << "(" << node->getData() << ")" << std::endl;

			// enter the next tree level - left and right branch
			printTree(prefix + (isLeft ? "|   " : "    "), node->getLeft(), true);
			printTree(prefix + (isLeft ? "|   " : "    "), node->getRight(), false);
		}
	}

	inline std::tuple<BinarySearchTreeNode<T>*, BinarySearchTreeNode<T>*> findInorderSuccessorHelper(
		BinarySearchTreeNode<T>* parentNode,
		BinarySearchTreeNode<T>* currNode
	)
	{
		if (currNode != nullptr)
		{
			if (currNode->hasLeft())
			{
				return findInorderSuccessorHelper(currNode, currNode->getLeft());
			}
			else
			{
				return std::make_tuple(parentNode, currNode);
			}
		}
		return std::make_tuple(nullptr, nullptr);
	}

	inline void setChildFromParent(
		BinarySearchTreeNode<T>* parentNode,
		BinarySearchTreeNode<T>* childToSet,
		BinarySearchTreeNode<T>* newRefToSetTo
	)
	{
		if (parentNode != nullptr)
		{
			if (parentNode->getLeft() == childToSet)
			{
				parentNode->setLeft(newRefToSetTo);
			}
			else if (parentNode->getRight() == childToSet)
			{
				parentNode->setRight(newRefToSetTo);
			}
		}
	}
	
	void cleanUpTree(BinarySearchTreeNode<T>* currNode)
	{
		// Post-order traversal to delete and free up memory taken by each node.
		// First the left three and right tree are visited and deleted first and then the current node is deleted so 
		// that everything descendent node is deleted first before deleting the current node to not lose reference
		// and cause memory leaks.
		if (currNode != nullptr)
		{
			if (currNode->hasLeft())
			{
				cleanUpTree(currNode->getLeft());
			}

			if (currNode->hasRight())
			{
				cleanUpTree(currNode->getRight());
			}

			delete currNode;
		}
	}

private:
	BinarySearchTreeNode<T>* root;
};