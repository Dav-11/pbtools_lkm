package compile

import (
	"fmt"
)

type OutputType string

const (
	Json      OutputType = "json"
	Netfilter OutputType = "netfilter"
	UDPSocket OutputType = "udp_sock"
)

var OutputTypeF OutputTypeFlag

type OutputTypeFlag struct {
	Value   OutputType
	Allowed []OutputType
}

func (f *OutputTypeFlag) Type() string {

	return "OutputType"
}

func (f *OutputTypeFlag) Set(s string) error {

	for _, allowed := range f.Allowed {
		if allowed == OutputType(s) {
			f.Value = allowed
			return nil
		}
	}

	return fmt.Errorf("invalid value for flag: %s (allowed values: %v)", s, f.Allowed)
}

func (f *OutputTypeFlag) String() string {
	return string(f.Value)
}

func init() {
	OutputTypeF.Allowed = []OutputType{
		Json,
		Netfilter,
		UDPSocket,
	}

	OutputTypeF.Value = Json
}
