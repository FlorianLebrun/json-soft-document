
namespace SoftDocument {
	template <typename Document, typename Value>
	struct Item {
		Item* next;
		Value value;
	};

	template <typename Document, typename Value>
	struct ObjectArray : Object {

		typedef SoftDocument::Item<Document, Value> Item;

		Item* firstItem;
		Item* lastItem;
		uint32_t length;

		struct iterator {
			Item* item;
			inline iterator(ObjectArray* obj) {
				if (obj) this->item = obj->firstItem;
				else this->item = 0;
			}
			inline Item* begin() {
				return this->item;
			}
			inline Item* next() {
				_ASSERT(this->item != 0);
				return this->item = this->item->next;
			}
		};

		ObjectArray() {
			this->firstItem = 0;
			this->lastItem = 0;
			this->length = 0;
		}
		bool equals(ObjectArray* other) {
			if (this->length != other->length) return false;
			Item* itemX = this->firstItem;
			Item* itemY = other->firstItem;
			while (itemX && itemY) {
				if (!itemX->value.equals(&itemY->value)) return false;
				itemX = itemX->next;
				itemY = itemY->next;
			}
			return true;
		}
		int count() {
			return this->length;
		}
		ObjectArray* copy(Document* document) {
			ObjectArray* obj = document->createObjectArray();
			for (Item* itemX = this->firstItem; itemX; itemX = itemX->next) {
				obj->push_back(document)->value.set(&itemX->value);
			}
			return obj;
		}
		ObjectArray* duplicate(Document* document) {
			ObjectArray* obj = document->createObjectArray();
			for (Item* itemX = this->firstItem; itemX; itemX = itemX->next) {
				obj->push_back(document)->value.duplicate(&itemX->value);
			}
			return obj;
		}
		ObjectArray* subtract(ObjectArray* other, Document* document) {
			Item* itemX = this->firstItem;
			Item* itemY = other->firstItem;

			// Compute first subtract with empty result
			Value lastDiffValue(document);
			bool hasDiff = false;
			for (;;) {
				if (itemX && itemY) {
					lastDiffValue.subtract(&itemX->value, &itemY->value);
					if (lastDiffValue.typeID != TypeID::Undefined) {
						hasDiff = true;
						break;
					}
				}
				else {
					hasDiff = itemX != itemY;
					break;
				}
				itemX = itemX->next;
				itemY = itemY->next;
			}

			// Create a minimized array when difference found
			if (hasDiff) {
				ObjectArray* obj = document->createObjectArray();

				// Fill with first elements
				for (Item* prevX = this->firstItem; prevX != itemX; prevX = prevX->next) {
					obj->push_back(document)->value.copyMinimize(&prevX->value);
				}

				// Fill with last diff value
				if (lastDiffValue.typeID != TypeID::Undefined) {
					obj->push_back(document)->value.set(&lastDiffValue);
					itemX = itemX->next;
					itemY = itemY->next;
				}

				// Fill with last subtracted elements
				if (itemX && itemY) {
					itemX = itemX->next;
					itemY = itemY->next;
					while (itemX && itemY) {
						Item* item = obj->push_back(document);
						item->value.subtract(&itemX->value, &itemY->value);
						if (item->value.typeID == TypeID::Undefined) {
							item->value.copyMinimize(&itemX->value);
						}
						itemX = itemX->next;
						itemY = itemY->next;
					}
				}

				// Fill with minimized elements
				if (itemX) {
					do {
						obj->push_back(document)->value.copyMinimize(&itemX->value);
						itemX = itemX->next;
					} while (itemX);
				}
				return obj;
			}

			return 0;
		}
		Item* push_front(Document* document) {
			Item* item = document->createItem();
			item->next = this->firstItem;
			if (!this->firstItem) this->lastItem = item;
			this->firstItem = item;
			this->length++;
			return item;
		}
		Item* push_back(Document* document) {
			Item* item = document->createItem();
			item->next = 0;
			if (this->lastItem) this->lastItem->next = item;
			else this->firstItem = item;
			this->lastItem = item;
			this->length++;
			return item;
		}
		Item* get(intptr_t index) {
			Item* item;
			for (item = this->firstItem; item && index; item = item->next) index--;
			return item;
		}
	};
}