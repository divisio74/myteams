/*
** EPITECH PROJECT, 2026
** myftp
** File description:
** test_bufferiser.c
*/

#include <criterion/criterion.h>
#include <stdlib.h>
#include <string.h>
#include "bufferiser.h"

Test(bufferise_client_operations, stores_remaining_chunk_after_delimiter)
{
    client_state_t client = {0};
    char args[] = "USER pierre\r\nPASS 1234";

    bufferise_client_operations(&client, args, -1);
    cr_assert_not_null(client.output_buffer);
    cr_assert_str_eq(client.output_buffer, "PASS 1234");
    free(client.output_buffer);
}

Test(bufferise_client_operations, stores_full_chunk_when_no_delimiter)
{
    client_state_t client = {0};
    char args[] = "NOOP";

    bufferise_client_operations(&client, args, -1);
    cr_assert_not_null(client.output_buffer);
    cr_assert_str_eq(client.output_buffer, "NOOP");
    free(client.output_buffer);
}

Test(bufferise_client_operations, keeps_last_chunk_with_multiple_delimiters)
{
    client_state_t client = {0};
    char args[] = "USER foo\r\nPASS bar\r\nPWD";

    bufferise_client_operations(&client, args, -1);
    cr_assert_not_null(client.output_buffer);
    cr_assert_str_eq(client.output_buffer, "PWD");
    free(client.output_buffer);
}

Test(bufferise_client_operations, does_not_write_buffer_when_empty_string)
{
    client_state_t client = {0};
    char args[] = "";

    bufferise_client_operations(&client, args, -1);
    cr_assert_null(client.output_buffer);
}

Test(bufferise_client_operations, keeps_existing_buffer_if_no_remaining_data)
{
    client_state_t client = {0};
    char args[] = "USER foo\r\n";

    client.output_buffer = strdup("OLD");
    cr_assert_not_null(client.output_buffer);
    bufferise_client_operations(&client, args, -1);
    cr_assert_not_null(client.output_buffer);
    cr_assert_str_eq(client.output_buffer, "OLD");
    free(client.output_buffer);
}

Test(bufferise_client_operations, replaces_existing_buffer_with_new_chunk)
{
    client_state_t client = {0};
    char args[] = "USER foo\r\nLIST -la";

    client.output_buffer = strdup("OLD");
    cr_assert_not_null(client.output_buffer);
    bufferise_client_operations(&client, args, -1);
    cr_assert_not_null(client.output_buffer);
    cr_assert_str_eq(client.output_buffer, "LIST -la");
    free(client.output_buffer);
}

Test(bufferise_client_operations, returns_cleanly_on_null_inputs)
{
    client_state_t client = {0};
    char args[] = "NOOP";

    bufferise_client_operations(NULL, args, -1);
    bufferise_client_operations(&client, NULL, -1);
    cr_assert_null(client.output_buffer);
}

/* Intentionally failing tests to expose parser edge-case bugs. */
Test(bufferise_client_operations, should_split_when_delimiter_is_at_start)
{
    client_state_t client = {0};
    char args[] = "\r\nPASS 1234";

    bufferise_client_operations(&client, args, -1);
    cr_assert_not_null(client.output_buffer);
    cr_assert_str_eq(client.output_buffer, "PASS 1234");
    free(client.output_buffer);
}

Test(bufferise_client_operations, should_support_standard_crlf_order)
{
    client_state_t client = {0};
    char args[] = "USER foo\r\nPASS 1234";

    bufferise_client_operations(&client, args, -1);
    cr_assert_not_null(client.output_buffer);
    cr_assert_str_eq(client.output_buffer, "PASS 1234");
    free(client.output_buffer);
}

Test(bufferise_client_operations, should_skip_empty_command_between_delimiters)
{
    client_state_t client = {0};
    char args[] = "A\r\n\r\nB";

    bufferise_client_operations(&client, args, -1);
    cr_assert_not_null(client.output_buffer);
    cr_assert_str_eq(client.output_buffer, "B");
    free(client.output_buffer);
}

Test(bufferise_client_operations, prepends_existing_output_buffer_to_new_data)
{
    client_state_t client = {0};
    char args[] = "pierre";

    client.output_buffer = strdup("USER ");
    cr_assert_not_null(client.output_buffer);
    bufferise_client_operations(&client, args, -1);
    cr_assert_not_null(client.output_buffer);
    cr_assert_str_eq(client.output_buffer, "USER pierre");
    free(client.output_buffer);
}

Test(bufferise_client_operations, prepended_buffer_is_consumed_before_crlf)
{
    client_state_t client = {0};
    char args[] = "pierre\r\nPASS 1234";

    client.output_buffer = strdup("USER ");
    cr_assert_not_null(client.output_buffer);
    bufferise_client_operations(&client, args, -1);
    cr_assert_not_null(client.output_buffer);
    cr_assert_str_eq(client.output_buffer, "PASS 1234");
    free(client.output_buffer);
}
