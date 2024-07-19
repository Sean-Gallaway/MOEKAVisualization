#include "Model.h"

Model::Model (GLFWwindow* m_window, int x, int y): window(m_window) {
	positionX = x;
	positionY = y;
	Window::addToRender(this);
};

Model::~Model () {
	// free the disks
	for (auto a : disks) {
		for (auto b : *a) {
			delete(b);
		}
		delete(a);
	}
}

void Model::draw () {
	for (auto a : disks) {
		if (a != nullptr) {
			for (auto b : *a) {
				b->draw();
			}
		}
	}
}

void Model::createDisk (std::vector<int>& values) {
	std::vector<Disk*>* temp = new std::vector<Disk*>;

	int elements = (int)ceil((double)values.size() / config::compressBarAmount);
	int stride = 0;

	for (int a = 0; a < elements; a++) {
		Disk* disk = new Disk{window, sizeX*config::compressBarAmount, sizeY, positionX, positionY + (sizeY+padding)*numOfDisks };
		// fill the disk segment
		for (int b = 0; b < config::compressBarAmount; b++) {
			Square* s = new Square(sizeX, sizeY);
			
			//
			s->kValue = (a*config::compressBarAmount) + b;
			//

			disk->addShape(*s);
		}
		// set positions
		disk->positionX = positionX + stride;
		disk->positionY = positionY + ((sizeY + padding) * numOfDisks);

		// set transforms
		disk->setTranslation(positionX + stride, positionY + ((sizeY + padding) * numOfDisks));
		disk->setScale(1);

		// update variables
		stride += sizeX*config::compressBarAmount;
		temp->insert(temp->end(), disk);
	}
	numOfDisks++;
	disks.insert(disks.end(), temp);
}

Shape* Model::getBar(int disk, int index) {
	if (disk > disks.size()) {
		std::cout << "Invalid disk" << std::endl;
		return nullptr;
	}
	if (index > disks.at(disk)->size()*config::compressBarAmount) {
		std::cout << "Invalid index on disk" << std::endl;
		return nullptr;
	}
	std::cout << index / config::compressBarAmount << "\t" << index % config::compressBarAmount << std::endl;
	return disks.at(disk)->at(floor(index / config::compressBarAmount))->managedList.at(index % config::compressBarAmount);
}

int Model::getX () {
	return positionX;
}

int Model::getY () {
	return positionY;
}

void Model::setX (float value) {
	positionX = value;
	int c = 0;
	for (auto a : disks) {
		for (auto b : *a) {
			b->setX(b->getX() + value + c);
			c += b->getSizeX();
		}
		c = 0;
	}
}

void Model::setY (float value) {
	positionY = value;
	int c = 0;
	for (auto a : disks) {
		for (auto b : *a) {
			b->setY(b->getY() + value + ((sizeY + padding) * c));
		}

		// heh
		c++;
	}
}

void Model::setScale (float scale) {
	totalScale = scale;
	for (auto a : disks) {
		for (auto b : *a) {
			b->setScale(scale);
		}
	}
	setTranslation(positionX, positionY);
}

void Model::setTranslation (float x, float y) {
	int c = 0;
	int d = 0;
	for (auto a : disks) {
		for (auto b : *a) {
			b->setTranslation(x + d, y + c);
			d += b->getSizeX();
		}
		d = 0;
		c += a->at(0)->getSizeY() + padding;
	}
}

Shape* Model::selectedShape() {
	Shape* s = nullptr;
	// model layer
	for (auto a : disks) {
		// disk layer
		for (auto b : *a) {
			s = b->selectedShape();
			if (s != nullptr) {
				return s;
			}
		}
	}
	return nullptr;
}


