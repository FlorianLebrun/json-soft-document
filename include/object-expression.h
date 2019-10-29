
namespace SoftDocument {
	template <typename Document, typename Value, typename ObjectString>
	struct ObjectExpression : ObjectArray<Document, Value> {
		ObjectString* name;
	};
}