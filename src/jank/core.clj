(ns jank.core
  (:gen-class)
  (:require [jank.parse :as parse]
            [jank.codegen.c++ :as c++]
            [jank.type.check :as type])
  (:use jank.debug.log))

(defn -main
  [& args]
  (-> {:cells (parse/parse
                (when (not-any? #(= "--bare" %) args)
                  parse/prelude)
                (slurp (first args)))}
      type/check
      clean-scope
      pprint
      ;c++/codegen
      ))
