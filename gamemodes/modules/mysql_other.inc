//Credits for promsters

stock cache_get(row_idx, const column_name[], destination[], max_len = sizeof(destination))
{
    cache_get_value_name(row_idx, column_name, destination, max_len);
}

stock cache_get_int(row_idx, const column_name[])
{
    new integer;
    cache_get_value_name_int(row_idx, column_name, integer);

    return integer;
}

stock Float:cache_get_float(row_idx, const column_name[])
{
    new Float:value;
    cache_get_value_name_float(row_idx, column_name, value);

    return value;
}

stock bool:cache_get_bool(row_idx, const column_name[])
{
    new bool:value;
    cache_get_value_name_int(row_idx, column_name, _:value);

    return value;
}

stock bool:cache_is_null(row_idx, const column_name[])
{
    new bool:isval_null;
    cache_is_value_name_null(row_idx, column_name, isval_null);

    return isval_null;
}

stock cache_get_rows()
{
    new count;
    cache_get_row_count(count);
    return count;
}

//---

stock cache_get_data(row_idx, const str[])
{
    new temp[256];
    cache_get(row_idx, str, temp);

    return _:temp;
}