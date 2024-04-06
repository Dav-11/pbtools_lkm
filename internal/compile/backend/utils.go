package backend

import "unicode"

func CamelCaseToSnakeCase(text string) string {

	var newStr []rune = nil
	isUpper := false

	for _, char := range text {

		if isUpper && char >= 'a' && char <= 'z' {
			newStr = append(newStr, '_')
		}

		newStr = append(newStr, unicode.ToLower(char))
		isUpper = unicode.IsUpper(char)
	}
	return string(newStr)
}
