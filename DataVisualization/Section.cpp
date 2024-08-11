#include "Section.h"

Section::Section (): Shape (0, 0) {
	color = { .5, .5, .5, 1 };
}

Section::~Section () {
	for (auto a : managedList) {
		delete(a);
	}
}

// adds a drawable object to this objects managedList, then calculates what this objects compressed color should be.
void Section::addChild (Drawable* child) {
	managedList.push_back(child);
	// for x/y, we calculate the translation by using its current x/y, the stride x/y, half this objects width/height, and half the child drawables width/height.
	child->setTranslation(strideX + getX() - (getWidth() * 0.5f) + (child->getWidth() * 0.5f), strideY + getY() - (getHeight() * 0.5f) + (child->getHeight() * 0.5f));
	if (horizontal) {
		strideX += child->getWidth() + paddingX;
	}
	else {
		strideY += child->getHeight() + paddingY;
	}
	
	// average color for background
	color = {0, 0, 0, 0};
	float width = 0;
	float maxWidth = 0;
	float height = 0;
	float maxHeight = 0;
	Drawable* temp;

	for (int a = 0; a < managedList.size(); a++) {
		temp = managedList.at(a);
		color.x += temp->getR();
		color.y += temp->getG();
		color.z += temp->getB();
		color.w += temp->getA();

		if (a != 0) {
			width+=paddingX*totalScale;
			height+=paddingY*totalScale;
		}

		width += temp->getWidth();
		maxWidth = std::max(temp->getWidth(), maxWidth);
		height += temp->getHeight();
		maxHeight = std::max(temp->getHeight(), maxHeight);
	}

	color.x /= (float)managedList.size();
	color.y /= (float)managedList.size();
	color.z /= (float)managedList.size();
	color.w /= (float)managedList.size();


	if (horizontal) {
		resize(width, maxHeight);
	}
	else {
		resize(maxWidth, height);
	}
	
}

// changes where on the screen this object gets rendered, and the rendering positions of its children.
void Section::setTranslation (float dx, float dy) {
	// compute some variables then set the position matrix
	x = dx;
	y = dy;
	dx = 2 * (dx / config::windowX) - 1;
	dy = 2 * (dy / config::windowY) - 1;
	translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(dx, -dy, 0));
	shader->use();
	shader->setUniformMat4f("posMatrix", translateMatrix);

	// reset the stride then set the translations again for each child drawable.
	strideX = 0;
	strideY = 0;
	Drawable* temp;

	for (int a = 0; a < managedList.size(); a++) {
		temp = managedList.at(a);
		if (temp == nullptr) {
			continue;
		}
		// we calculate the translation by using its current x/y, the stride x/y, half this objects width/height, and half the child drawables width/height.
		temp->setTranslation(strideX + x - (getWidth() * 0.5f) + (temp->getWidth() * 0.5f), strideY + y - (getHeight() * 0.5f) + (temp->getHeight() * 0.5f));
		if (horizontal) {
			strideX += temp->getWidth()+(paddingX*totalScale);
		}
		else {
			strideY += temp->getHeight()+(paddingY*totalScale);

		}
	}
}

// sets the scale of this object, and the scale of its children.
void Section::setScale (float scale) {
	totalScale = scale;
	scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));
	shader->use();
	shader->setUniformMat4f("scaleMatrix", scaleMatrix);
	scaleBounds(scale);

	for (auto a : managedList) {
		a->setScale(scale);
		compress = a->getWidth() <= config::compressValue;
	}
	setTranslation(getX(), getY());
}

// draws this object and its children.
void Section::draw () {
	if (compress) {
		color.x = 0;
		color.y = 0;
		color.w = 0;
		color.z = 0;
		for (auto a : managedList) {
			color.x += a->getR();
			color.y += a->getG();
			color.z += a->getB();
			color.w += a->getA();
		}
		color.x /= (float)managedList.size();
		color.y /= (float)managedList.size();
		color.z /= (float)managedList.size();
		color.w /= (float)managedList.size();
		bind();
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
		return;
	}
	for (auto a : managedList) {
		a->draw();
	}
}

Drawable* Section::selected (GLFWwindow* window) {
	// use BVH-esque techniques to find the hovered shape without checking too many shapes.
	double cx, cy;
	glfwGetCursorPos(window, &cx, &cy);


	// cx and cy will be in ranged of 0-600 from 0,0 on the top right to 600, 600, on the bottom right
	// check if cursor is in the bounds of this manager.
	double tempx = (double)getWidth() / 2.0f;
	double tempy = (double)getHeight() / 2.0f;
	if (getX() - tempx > cx || cx > getX() + tempx) {
		return nullptr;
	}
	if (getY() - tempy > cy || cy > getY() + tempy) {
		return nullptr;
	}

	// checks against a shapes bounding box.
	Drawable* d;
	for (auto s : managedList) {
		d = s->selected(window);
		if (d != nullptr) {
			return d;
		}
	}
	return nullptr;
}