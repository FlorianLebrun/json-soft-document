
namespace SoftDocument {
	template <typename Document, typename Value, typename ObjectSymbol>
	struct ObjectExpression : ObjectArray<Document, Value> {
		ObjectSymbol* name;
	};
}