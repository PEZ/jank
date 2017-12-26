(ns com.jeaye.jank.parse
  (:require [instaparse.core :as insta]
            [com.jeaye.jank
             [log :refer [pprint]]
             [assert :refer [parse-assert]]]
            [com.jeaye.jank.parse
             [transform :as transform]]))

(insta/defparser whitespace-or-comments-parser
  (clojure.java.io/resource "neo-whitespace-grammar"))

(insta/defparser parser
  (clojure.java.io/resource "neo-grammar")
  :auto-whitespace whitespace-or-comments-parser)

(defn add-meta [input parsed]
  (insta/add-line-and-column-info-to-metadata input parsed))

(defn parse
  "Runs the provided resource file through instaparse and transforms from hiccup
   to hickory. Returns the generated syntax tree."
  [prelude file input]
  ;(pprint "parsing" input)
   (let [parsed (pprint "parsed raw" (parser input))
         error (pr-str (insta/get-failure parsed))
         _ (parse-assert (not (insta/failure? parsed))
                         "invalid syntax\n" error)
         parsed-with-meta (add-meta input parsed)
         _ (pprint "parsed" parsed-with-meta)
         transformed (binding [transform/*input-file* file]
                       (insta/transform
                         {:integer (partial transform/read-single :integer)
                          :real (partial transform/read-single :real)
                          :boolean (partial transform/read-single :boolean)
                          :keyword (partial transform/keyword :unqualified)
                          :qualified-keyword (partial transform/keyword :qualified)
                          :string (partial transform/single :string)
                          :map transform/map
                          :identifier (partial transform/single :identifier)
                          :binding-definition transform/binding-definition
                          :application transform/application
                          :fn-definition transform/fn-definition
                          :argument-list transform/argument-list}
                         parsed-with-meta))]
     ;(pprint "transformed" transformed)
     (pprint (meta (-> transformed first :name)))
     {::file file
      ::tree (into prelude transformed)}))

(defn parses [source & args]
  (apply insta/parses parser source args))

(def prelude (->> (clojure.java.io/resource "neo-prelude.jank")
                  slurp
                  (parse [] "<prelude>")
                  ::tree))