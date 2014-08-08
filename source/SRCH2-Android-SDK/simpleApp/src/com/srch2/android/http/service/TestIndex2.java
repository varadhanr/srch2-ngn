package com.srch2.android.http.service;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

/**
 * Created by jianfeng on 8/4/14.
 */
public class TestIndex2 extends TestIndex {
    public static final String INDEX_NAME = "test2";

    /**
     * Returns a core named "test" with fields "id" as primary key and "title" as searchable text title.
     */
    @Override
    public IndexDescription getIndexDescription() {
        Field primaryKey = Field.createSearchableField(INDEX_FIELD_NAME_PRIMARY_KEY);
        Field title = Field.createSearchableField(INDEX_FIELD_NAME_TITLE);
        Field score = Field.createRefiningField(INDEX_FIELD_NAME_SCORE, Field.Type.INTEGER);

        return new IndexDescription(INDEX_NAME, primaryKey, title, score);
    }

    /**
     * Returns a set of records with "id" field incremented by for loop and "title" set to "Title# + <loopIteration>".
     */
    public JSONArray getRecordsArray(int numberOfRecordsToInsert, int primaryKeyStartIndice) {
        JSONArray recordsArray = new JSONArray();
        for (int i = primaryKeyStartIndice; i < numberOfRecordsToInsert + primaryKeyStartIndice; ++i) {
            JSONObject recordObject = new JSONObject();
            try {
                recordObject.put(INDEX_FIELD_NAME_PRIMARY_KEY, String.valueOf(i));
                recordObject.put(INDEX_FIELD_NAME_TITLE, "Title ");
                recordObject.put(INDEX_FIELD_NAME_SCORE, i);
            } catch (JSONException ignore) {
            }
            recordsArray.put(recordObject);
        }
        return recordsArray;
    }
}
