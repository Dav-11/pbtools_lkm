package model

type Enum struct {
	Name          string
	NameSnakeCase string
}

type EnumMember struct {
	NameSnakeCase string
	FieldNumber   int
}
