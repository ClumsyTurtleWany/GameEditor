// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: enum.proto

#include "enum.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG
namespace protocol {
}  // namespace protocol
static const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* file_level_enum_descriptors_enum_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_enum_2eproto = nullptr;
const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_enum_2eproto::offsets[1] = {};
static constexpr ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema* schemas = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::Message* const* file_default_instances = nullptr;

const char descriptor_table_protodef_enum_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\nenum.proto\022\010protocol*\202\001\n\nPlayerType\022\024\n"
  "\020PLAYER_TYPE_NONE\020\000\022\026\n\022PLAYER_TYPE_KNIGH"
  "T\020\001\022\024\n\020PLAYER_TYPE_MAGE\020\002\022\026\n\022PLAYER_TYPE"
  "_ARCHER\020\003\022\030\n\024PLAYER_TYPE_ASSASSIN\020\004*o\n\007C"
  "ardNum\022\n\n\006Strike\020\000\022\n\n\006Defend\020\001\022\017\n\013PomelS"
  "trike\020\002\022\016\n\nShrugItOff\020\003\022\017\n\013Hemokinesis\020\004"
  "\022\014\n\010BludGeon\020\005\022\014\n\010IronWave\020\006b\006proto3"
  ;
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_enum_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_enum_2eproto = {
  false, false, 276, descriptor_table_protodef_enum_2eproto, "enum.proto", 
  &descriptor_table_enum_2eproto_once, nullptr, 0, 0,
  schemas, file_default_instances, TableStruct_enum_2eproto::offsets,
  nullptr, file_level_enum_descriptors_enum_2eproto, file_level_service_descriptors_enum_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable* descriptor_table_enum_2eproto_getter() {
  return &descriptor_table_enum_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY static ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptorsRunner dynamic_init_dummy_enum_2eproto(&descriptor_table_enum_2eproto);
namespace protocol {
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* PlayerType_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_enum_2eproto);
  return file_level_enum_descriptors_enum_2eproto[0];
}
bool PlayerType_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
      return true;
    default:
      return false;
  }
}

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* CardNum_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_enum_2eproto);
  return file_level_enum_descriptors_enum_2eproto[1];
}
bool CardNum_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      return true;
    default:
      return false;
  }
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace protocol
PROTOBUF_NAMESPACE_OPEN
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
